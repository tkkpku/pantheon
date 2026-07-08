#include "pantheon.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>

// ============================================================
// 混凝土密度（四层阶梯，从顶部浮石到底部石灰华）
// 数据来源: Moore (1995); Aliberti & Alonso-Rodríguez (2017)
// ============================================================
double density(double phi) {
    double deg = phi * 180.0 / PI;        // 弧度 → 角度，从球顶起算
    if (deg < 12.0)      return 1350.0;    // 顶部：纯浮石（Pumice）
    else if (deg < 25.0) return 1600.0;    // 第二层：浮石+凝灰岩
    else if (deg < 40.0) return 1800.0;    // 第三层：凝灰岩+砖块
    else                 return 2100.0;    // 底部：石灰华（Travertine）
}

// ============================================================
// 穹顶厚度——从 Oculus 边缘到底部线性渐变
// t(φ) = T_OCULUS + (T_BASE - T_OCULUS) * (φ - φ_oc) / (π/2 - φ_oc)
// ============================================================
double thickness(double phi) {
    double oc = asin(R_OCULUS / R), mx = PI / 2.0;
    if (phi <= oc) return T_OCULUS;       // Oculus 以上保持最薄
    if (phi >= mx) return T_BASE;         // 鼓座底部达到最厚
    return T_OCULUS + (T_BASE - T_OCULUS) * (phi - oc) / (mx - oc);
}

// ============================================================
// Simpson 数值积分（复合公式，n 分段数，须为偶数）
// ∫_a^b f(x)dx ≈ h/3 · [f₀ + 4f₁ + 2f₂ + 4f₃ + ... + f_n]
// ============================================================
double simpson(double (*f)(double), double a, double b, int n) {
    if (n % 2 != 0) n++;                  // n 必须为偶数
    double h = (b - a) / n, s = f(a) + f(b);  // 首尾权重 = 1
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        s += (i % 2 == 0) ? 2.0 * f(x) : 4.0 * f(x);  // 奇→×4, 偶→×2
    }
    return s * h / 3.0;
}

// ============================================================
// 球壳薄膜内力（经典解，仅供应力分布形状参考）
// N_φ(φ) = -gR / (1+cos φ)          —— 子午向薄膜力
// N_θ(φ) = -gR [cos φ - 1/(1+cos φ)] —— 环向薄膜力
// 参考: Calladine (1983) Theory of Shell Structures
// 注: 万神殿厚跨比 t/R ≈ 0.27，薄膜解绝对值不准确（高估~3.5倍）
//     但应力分布形状及其相对变化（Δσ/σ ≈ -B）是可靠的
// ============================================================
double N_phi_val(double phi) {
    double g = density(phi) * thickness(phi) * G;
    return -g * R / (1.0 + cos(phi));
}

double N_theta_val(double phi) {
    double g = density(phi) * thickness(phi) * G;
    return -g * R * (cos(phi) - 1.0 / (1.0 + cos(phi)));
}

double sigma_theta(double phi) {
    return N_theta_val(phi) / thickness(phi);
}

// ============================================================
// 无藻井穹顶总质量：一维 Simpson 积分沿经线方向
// M = ∫ ρ(φ) · t(φ) · 2πR²·sin φ dφ
// n=200 经收敛性验证，相对误差约 0.0007%
// ============================================================
static double dM_dphi(double phi) {
    return density(phi) * thickness(phi) * 2.0 * PI * R * R * sin(phi);
}

double total_mass_no_coffers() {
    return simpson(dM_dphi, asin(R_OCULUS / R), PI / 2.0, 200);
}

// ============================================================
// 单个藻井体积：平底棱台公式
// V = d/3 · (S_top + S_bottom + √(S_top·S_bottom))
// S_top = w_top · arc_height（开口面，近似矩形）
// S_bottom = (w_bottom·s_c) · (arc_height·s_c)（底面，径向修正后）
// 径向修正因子 s_c = (R+d)/R —— 反映罗马垂直模板施工几何
// ============================================================
double single_coffer_volume_prism(double pc,
    double ah, double wb, double wt, double d)
{
    double St = wt * ah;                     // 上底面积（开口面）
    double sc = (R + d) / R;                 // 径向修正因子
    double Sb = (wb * sc) * (ah * sc);       // 下底面积（修正后）
    return (d / 3.0) * (St + Sb + sqrt(St * Sb)); // 棱台公式
}

// ============================================================
// 拱券模型——开裂后状态
// ============================================================

// 单个楔形拱自重：总质量·g / 28 · (1 - wr)
// wr = B = M_coffers / M_total（减重比例）
double arch_self_weight(double wr) {
    return total_mass_no_coffers() * G * (1.0 - wr) / 28.0;
}

// 拱脚水平推力 H = k · W_arch
// k ≈ (1 - sin(φ_oc)) / (2 · cos(φ_oc))（简化推力线分析）
// ⚠️ 警告：简化公式 k ≈ 0.407，精确3D积分值 k ≈ 0.321，绝对值误差约 21%！
//      JCIR = B 作为比值不受此误差影响（k 在分子分母中抵消），
//      但返回的推力绝对值（~3.5 MN）高估了约 21%，不可作为真实推力使用。
//      如需精确绝对值请参考 MATLAB 3D 数值积分结果（fig06_k_invariance）。
double arch_horizontal_thrust(double wr) {
    double W = arch_self_weight(wr);
    double phi_oc = asin(R_OCULUS / R);
    double k = (1.0 - sin(phi_oc)) / (2.0 * cos(phi_oc));
    return k * W;
}

// JCF = H / DRUM_HOOP_CAPACITY（归一化交界处裂缝因子）
// 仅相对变化 JCIR 有物理意义
double compute_jcf(double wr) {
    return arch_horizontal_thrust(wr) / DRUM_HOOP_CAPACITY;
}

// JCIR = (JCF₀ - JCF_B)/JCF₀ = (H₀ - H_B)/H₀ = B
// 力学恒等式：k 在分子分母中完全抵消
double compute_jcir(double wr) {
    double jcf0 = compute_jcf(0.0);
    double jcfB = compute_jcf(wr);
    return (jcf0 - jcfB) / jcf0;
}

// ============================================================
// 子午向裂缝分析
// ============================================================

// 底部有效环向拉应力 = 薄膜应力 × (1-B) + 收缩等效拉应力
// σ_sh ≈ 182 kPa 由裂缝终止于 57° 的标定条件反算
double peak_hoop_stress_base(double wr) {
    double st_film = sigma_theta(PI / 2.0);
    return st_film * (1.0 - wr) + SIGMA_SHRINKAGE;
}

// MCFR = (σ₀ - σ_B)/σ₀
// 由于收缩 σ_sh 不受自重影响，MCFR < JCIR
double compute_mcfr(double wr) {
    double s0 = peak_hoop_stress_base(0.0);
    double sB = peak_hoop_stress_base(wr);
    return (s0 - sB) / s0;
}

// ============================================================
// 质量计算
// ============================================================

CofferResult compute_coffers(const std::vector<CofferLayer>& layers) {
    CofferResult res;
    res.total_coffer_volume = 0.0;
    res.total_coffer_mass   = 0.0;
    int n = (int)layers.size();
    res.layer_volumes.resize(n);
    res.layer_masses.resize(n);

    for (int i = 0; i < n; i++) {
        const CofferLayer& L = layers[i];
        double V1 = single_coffer_volume_prism(
            L.phi_center, L.arc_height, L.w_bottom, L.w_top, L.depth);
        double Vl = V1 * L.count;
        double Ml = Vl * density(L.phi_center);
        res.layer_volumes[i] = Vl;
        res.layer_masses[i]  = Ml;
        res.total_coffer_volume += Vl;
        res.total_coffer_mass   += Ml;
    }
    res.mass_no_coffers      = total_mass_no_coffers();
    res.mass_reduction_ratio = res.total_coffer_mass / res.mass_no_coffers;
    return res;
}

// ============================================================
// CSV 导出
// ============================================================

void export_stress_profile(const char* fn, int n) {
    std::ofstream f(fn);
    f << "phi_apex_deg,phi_horiz_deg,thickness_m,density_kgm3,"
      << "N_phi_MNpm,N_theta_MNpm,sigma_theta_kPa,sigma_eff_kPa,"
      << "ft_ref_kPa\n";
    double oc = asin(R_OCULUS / R), mx = PI / 2.0;
    for (int i = 0; i <= n; i++) {
        double phi = oc + (mx - oc) * i / n;
        double st  = sigma_theta(phi);
        double se  = st + SIGMA_SHRINKAGE;  // 有效应力 = 薄膜应力 + 收缩
        f << phi * 180.0 / PI << ","
          << (PI / 2.0 - phi) * 180.0 / PI << ","
          << thickness(phi) << "," << density(phi) << ","
          << N_phi_val(phi) / 1e6 << ","
          << N_theta_val(phi) / 1e6 << ","
          << st / 1e3 << "," << se / 1e3 << ","
          << FT_DOME / 1e3 << "\n";
    }
    f.close();
}

void export_layer_details(const char* fn,
    const std::vector<CofferLayer>& layers, const CofferResult& res)
{
    std::ofstream f(fn);
    f << "层,纬度deg,弧高m,底宽m,顶宽m,深度m,数量,单个体积m3,层体积m3,层质量吨\n";
    for (size_t i = 0; i < layers.size(); i++) {
        const CofferLayer& L = layers[i];
        double V1 = single_coffer_volume_prism(
            L.phi_center, L.arc_height, L.w_bottom, L.w_top, L.depth);
        f << L.name << "," << L.phi_center * 180.0 / PI << ","
          << L.arc_height << "," << L.w_bottom << "," << L.w_top << ","
          << L.depth << "," << L.count << ","
          << V1 << "," << res.layer_volumes[i] << ","
          << res.layer_masses[i] / 1000.0 << "\n";
    }
    f.close();
}

void export_arch_scan(const char* fn,
    const std::vector<double>& factors,
    const std::vector<double>& Bv,
    const std::vector<double>& jcfv,
    const std::vector<double>& jcirv,
    const std::vector<double>& peak_stress,
    const std::vector<double>& mcfrv)
{
    std::ofstream f(fn);
    f << "factor,B_percent,JCF,JCIR_percent,peak_stress_kPa,"
      << "MCFR_percent\n";
    for (size_t i = 0; i < factors.size(); i++) {
        f << factors[i] << ","
          << Bv[i] * 100.0 << ","
          << jcfv[i] << ","
          << jcirv[i] * 100.0 << ","
          << peak_stress[i] / 1e3 << ","
          << mcfrv[i] * 100.0 << "\n";
    }
    f.close();
}
