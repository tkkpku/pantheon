#include "pantheon.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>

double density(double phi) {
    double deg = phi * 180.0 / PI;
    if (deg < 12.0)      return 1350.0;
    else if (deg < 25.0) return 1600.0;
    else if (deg < 40.0) return 1800.0;
    else                 return 2100.0;
}

double thickness(double phi) {
    double oc = asin(R_OCULUS / R), mx = PI / 2.0;
    if (phi <= oc) return T_OCULUS;
    if (phi >= mx) return T_BASE;
    return T_OCULUS + (T_BASE - T_OCULUS) * (phi - oc) / (mx - oc);
}

double simpson(double (*f)(double), double a, double b, int n) {
    if (n % 2 != 0) n++;
    double h = (b - a) / n, s = f(a) + f(b);
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        s += (i % 2 == 0) ? 2.0 * f(x) : 4.0 * f(x);
    }
    return s * h / 3.0;
}

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

static double dM_dphi(double phi) {
    return density(phi) * thickness(phi) * 2.0 * PI * R * R * sin(phi);
}

double total_mass_no_coffers() {
    return simpson(dM_dphi, asin(R_OCULUS / R), PI / 2.0, 200);
}

double single_coffer_volume_prism(double pc,
    double ah, double wb, double wt, double d)
{
    double St = wt * ah;
    double sc = (R + d) / R;
    double Sb = (wb * sc) * (ah * sc);
    return (d / 3.0) * (St + Sb + sqrt(St * Sb));
}

// ============================================================
// 拱券模型
// ============================================================

double arch_self_weight(double wr) {
    return total_mass_no_coffers() * G * (1.0 - wr) / 28.0;
}

double arch_horizontal_thrust(double wr) {
    // H = k · W_arch
    // k ≈ cot(α_eff) / 2 ≈ 0.407
    // α_eff: 推力线在拱脚处的等效倾角
    double W = arch_self_weight(wr);
    double phi_oc = asin(R_OCULUS / R);
    double k = (1.0 - sin(phi_oc)) / (2.0 * cos(phi_oc));
    return k * W;
}

double compute_jcf(double wr) {
    return arch_horizontal_thrust(wr) / DRUM_HOOP_CAPACITY;
}

double compute_jcir(double wr) {
    double jcf0 = compute_jcf(0.0);
    double jcfB = compute_jcf(wr);
    return (jcf0 - jcfB) / jcf0;
}

// ============================================================
// 子午向裂缝分析
// ============================================================

double peak_hoop_stress_base(double wr) {
    // 底部 (φ_apex=90°) 的有效环向拉应力
    double st_film = sigma_theta(PI / 2.0);
    return st_film * (1.0 - wr) + SIGMA_SHRINKAGE;
}

double compute_mcfr(double wr) {
    // 底部有效拉应力的降低比例
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
        double se  = st + SIGMA_SHRINKAGE;
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
