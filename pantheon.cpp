#include "pantheon.h"
#include <iostream>
#include <fstream>
#include <iomanip>

// ============================================================
// 密度（四层梯度）
// ============================================================
double density(double phi) {
    double deg = phi * 180.0 / PI;
    if (deg < 12.0)      return 1350.0;   // 纯浮石
    else if (deg < 25.0) return 1600.0;   // 浮石+凝灰岩
    else if (deg < 40.0) return 1800.0;   // 凝灰岩+碎砖
    else                 return 2100.0;   // 石灰华
}

// ============================================================
// 厚度（线性递减）
// ============================================================
double thickness(double phi) {
    double phi_oc = asin(R_OCULUS / R);
    double phi_max = PI / 2.0;
    if (phi <= phi_oc) return T_OCULUS;
    if (phi >= phi_max) return T_BASE;
    double r = (phi - phi_oc) / (phi_max - phi_oc);
    return T_OCULUS + (T_BASE - T_OCULUS) * r;
}

// ============================================================
// Simpson
// ============================================================
double simpson(double (*f)(double), double a, double b, int n) {
    if (n % 2 != 0) n++;
    double h = (b - a) / n;
    double s = f(a) + f(b);
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        s += (i % 2 == 0) ? 2.0 * f(x) : 4.0 * f(x);
    }
    return s * h / 3.0;
}

// ============================================================
// 无藻井穹顶总质量
// ============================================================
static double dM_dphi(double phi) {
    double t   = thickness(phi);
    double rho = density(phi);
    return rho * t * 2.0 * PI * R * R * sin(phi);
}

double total_mass_no_coffers() {
    double phi_oc = asin(R_OCULUS / R);
    return simpson(dM_dphi, phi_oc, PI / 2.0, 200);
}

// ============================================================
// 单个藻井体积 —— 平底棱台公式
// ============================================================
// 藻井的四个侧面沿球面法线方向（近似径向）向内收拢。
// 底面（靠近穹顶外表面）比顶面（内表面开口）大。
//
// 底面积 ≈ (w_bottom × arc_height_bottom)
// 顶面积 ≈ (w_top    × arc_height_top)
//
// arc_height 在深度 d 处的修正：
//   弧长 ∝ 半径 → 深度 d 处的弧长 = arc_height × (R+d)/R
//   同理宽度也要修正：w_at_depth = w × (R+d)/R
//
// 但由于论文说藻井内表面是平的（不是沿球面的），
// 最简化的处理是：
//   底面积 S_bot = w_bottom × arc_height × (R+depth)/R
//   顶面积 S_top = w_top    × arc_height
//   V ≈ (depth/3) × (S_top + S_bot + sqrt(S_top × S_bot))
//
// 进一步简化：藻井深度 d ≪ R（0.5m vs 22m），
// 面积修正因子 (R+d)/R ≈ 1.02，可以忽略。
//   直接用 V ≈ (S_top + S_bot)/2 × depth
//
double single_coffer_volume_prism(double phi_center,
    double arc_height, double w_bottom, double w_top, double depth)
{
    // 顶面积（内表面开口）
    double S_top = w_top * arc_height;

    // 底面积：因深度增加而略微放大（径向发散）
    double scale = (R + depth) / R;
    double S_bot = (w_bottom * scale) * (arc_height * scale);

    // 棱台体积公式
    return (depth / 3.0) * (S_top + S_bot + sqrt(S_top * S_bot));
}

// ============================================================
// 主计算
// ============================================================
CofferResult compute_coffers(const std::vector<CofferLayer>& layers) {
    CofferResult res;
    res.total_coffer_volume = 0.0;
    res.total_coffer_mass   = 0.0;

    int n = layers.size();
    res.layer_volumes.resize(n);
    res.layer_masses.resize(n);

    for (int i = 0; i < n; i++) {
        const CofferLayer& L = layers[i];
        double V1 = single_coffer_volume_prism(
            L.phi_center, L.arc_height, L.w_bottom, L.w_top, L.depth);
        double V_layer = V1 * L.count;
        double rho_i   = density(L.phi_center);
        double M_layer = V_layer * rho_i;

        res.layer_volumes[i] = V_layer;
        res.layer_masses[i]  = M_layer;
        res.total_coffer_volume += V_layer;
        res.total_coffer_mass   += M_layer;
    }

    res.mass_no_coffers     = total_mass_no_coffers();
    res.mass_reduction_ratio = res.total_coffer_mass / res.mass_no_coffers;

    return res;
}

// ============================================================
// 导出应力剖面 CSV
// ============================================================
void export_stress_profile(const char* filename, int n) {
    std::ofstream f(filename);
    f << "phi_deg,thickness_m,density_kgm3,N_phi_MNpm,N_theta_MNpm,";
    f << "sigma_theta_MPa\n";

    double phi_oc = asin(R_OCULUS / R);
    double phi_max = PI / 2.0;
    for (int i = 0; i <= n; i++) {
        double phi = phi_oc + (phi_max - phi_oc) * i / n;
        double t   = thickness(phi);
        double rho = density(phi);
        double g   = rho * t * G;
        double N_phi   = -g * R / (1.0 + cos(phi));
        double N_theta = -g * R * (cos(phi) - 1.0/(1.0 + cos(phi)));
        double sig_th  = N_theta / t / 1e6;  // MPa

        f << phi * 180.0 / PI << ",";
        f << t << "," << rho << ",";
        f << N_phi / 1e6 << "," << N_theta / 1e6 << ",";
        f << sig_th << "\n";
    }
    f.close();
    std::cout << "[导出] " << filename << " (" << n+1 << " 行)\n";
}

// ============================================================
// 导出藻井各层详情 CSV
// ============================================================
void export_layer_details(const char* filename,
    const std::vector<CofferLayer>& layers,
    const CofferResult& res)
{
    std::ofstream f(filename);
    f << "层,纬度deg,弧高m,底宽m,顶宽m,深度m,数量,";
    f << "单个体积m3,层体积m3,层质量吨\n";

    for (size_t i = 0; i < layers.size(); i++) {
        const CofferLayer& L = layers[i];
        double V1 = single_coffer_volume_prism(
            L.phi_center, L.arc_height, L.w_bottom, L.w_top, L.depth);

        f << L.name << ",";
        f << L.phi_center * 180.0 / PI << ",";
        f << L.arc_height << ",";
        f << L.w_bottom << ",";
        f << L.w_top << ",";
        f << L.depth << ",";
        f << L.count << ",";
        f << V1 << ",";
        f << res.layer_volumes[i] << ",";
        f << res.layer_masses[i] / 1000.0 << "\n";
    }
    f.close();
    std::cout << "[导出] " << filename << "\n";
}
