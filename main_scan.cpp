#include "pantheon.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// ============================================================
// 各层抗压强度 (MPa → Pa)
// ============================================================
double fc_by_layer(int i) {
    double fc_MPa[5] = {12.0, 9.0, 7.0, 7.0, 5.0};
    return fc_MPa[i] * 1e6;
}

// ============================================================
// 计算某纬度处的 |N_phi| (N/m)
// ============================================================
double N_phi_abs(double phi) {
    double t   = thickness(phi);
    double rho = density(phi);
    double g   = rho * t * G;          // N/m²
    double N   = -g * R / (1.0 + cos(phi)); // 薄膜解（负=压缩）
    return fabs(N);
}

int main() {
    double depths_base[5] = {0.67, 0.65, 0.61, 0.54, 0.48};
    double factors[]      = {0.60, 0.80, 1.00, 1.20, 1.40};
    double band = 0.84;
    double h[5] = {4.08, 3.79, 3.52, 3.05, 2.39};
    double w_bot[5] = {3.88, 3.74, 3.39, 2.92, 2.38};
    double w_top[5] = {3.77, 3.48, 3.02, 2.50, 2.02};
    const char* names[5] = {"A", "B", "C", "D", "E"};

    // 各层藻井中心纬度
    double phi_oculus = asin(R_OCULUS / R);
    double cum = band / 2.0;
    double arc_from_cornice[5];
    for (int i = 0; i < 5; i++) {
        arc_from_cornice[i] = cum + h[i] / 2.0;
        cum += h[i] + band;
    }

    double E_base = 0.0;

    // ---- 表头 ----
    std::cout << std::fixed;
    std::cout << "因子   减重%    减重吨  ";
    for (int i = 0; i < 5; i++)
        std::cout << "  R_" << names[i] << "  ";
    std::cout << " Rmax   效率E   效率Δ%\n";
    std::cout << std::string(75, '-') << "\n";

    for (int fi = 0; fi < 5; fi++) {
        double factor = factors[fi];

        // 构建藻井列表
        std::vector<CofferLayer> layers;
        for (int i = 0; i < 5; i++) {
            CofferLayer L;
            L.name       = names[i];
            L.phi_center = PI / 2.0 - arc_from_cornice[i] / R;
            L.arc_height = h[i];
            L.w_bottom   = w_bot[i];
            L.w_top      = w_top[i];
            L.depth      = depths_base[i] * factor;
            L.count      = 28;
            layers.push_back(L);
        }

        // 计算减重
        CofferResult res = compute_coffers(layers);

        // 计算各层风险因子 R_i
        double R[5], Rmax = 0.0;
        for (int i = 0; i < 5; i++) {
            double phi_i   = layers[i].phi_center;
            double t_i     = thickness(phi_i);
            double d_i     = layers[i].depth;
            double t_res   = t_i - d_i;
            double N_phi_i = N_phi_abs(phi_i);
            double fc_i    = fc_by_layer(i);
            R[i] = N_phi_i / (fc_i * t_res);
            if (R[i] > Rmax) Rmax = R[i];
        }

        double B = res.mass_reduction_ratio * 100.0;  // %
        double E = B / Rmax;

        if (factor == 1.00) E_base = E;

        // 输出一行
        std::cout << std::setprecision(2);
        std::cout << std::setw(4)  << factor << "x"
                  << std::setw(8)  << B << "%"
                  << std::setw(9)  << std::setprecision(1)
                  << res.total_coffer_mass / 1000.0 << "t";

        for (int i = 0; i < 5; i++) {
            std::cout << std::setw(7) << std::setprecision(3) << R[i];
        }
        std::cout << std::setw(7) << std::setprecision(3) << Rmax;
        std::cout << std::setw(9) << std::setprecision(1) << E;

        if (factor == 1.00)
            std::cout << "  (基准)";
        else
            std::cout << std::setw(6) << std::setprecision(0)
                      << (E - E_base) / E_base * 100.0 << "%";

        std::cout << "\n";
    }

    std::cout << std::string(75, '-') << "\n";
    std::cout << "\n定义:\n";
    std::cout << "  R_i = |N_φ(φ_i)| / [f_c · (t(φ_i) - d_i)]\n";
    std::cout << "  R_i → 0: 安全;  R_i → 1: 局部压溃\n";
    std::cout << "  E = B / Rmax （单位风险换来的减重百分比）\n";
    std::cout << "  R 的非线性: R ∝ 1/(t-d)，d→t 时 R→∞\n";

    return 0;
}
