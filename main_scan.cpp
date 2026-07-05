#include "pantheon.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

int main() {
    double depths_base[5] = {0.67, 0.65, 0.61, 0.54, 0.48};
    double factors[]      = {0.60, 0.80, 1.00, 1.20, 1.40};
    double band = 0.84;
    double h[5]     = {4.08, 3.79, 3.52, 3.05, 2.39};
    double w_bot[5] = {3.88, 3.74, 3.39, 2.92, 2.38};
    double w_top[5] = {3.77, 3.48, 3.02, 2.50, 2.02};

    double phi_oculus = asin(R_OCULUS / R);
    double cum = band / 2.0;
    double arc[5];
    for (int i = 0; i < 5; i++) { arc[i] = cum + h[i]/2.0; cum += h[i] + band; }

    std::cout << std::fixed;
    std::cout << "==============================================\n";
    std::cout << "  藻井深度敏感性 — 拱券模型 & 两类裂缝\n";
    std::cout << "==============================================\n\n";

    std::vector<double> Bv, jcfv, jcirv, psv, mcfrv;
    double jcf_base = 0.0;

    std::cout << "因子    减重%    减重吨    ";
    std::cout << "JCF       JCIR%      ";
    std::cout << "底部应力kPa  MCFR%\n";
    std::cout << std::string(78, '-') << "\n";

    for (int fi = 0; fi < 5; fi++) {
        double factor = factors[fi];
        std::vector<CofferLayer> layers;
        for (int i = 0; i < 5; i++) {
            CofferLayer L;
            L.phi_center = PI/2.0 - arc[i]/R;
            L.arc_height = h[i]; L.w_bottom = w_bot[i];
            L.w_top = w_top[i]; L.depth = depths_base[i] * factor;
            L.count = 28;
            layers.push_back(L);
        }

        CofferResult res = compute_coffers(layers);
        double B    = res.mass_reduction_ratio;
        double jcf  = compute_jcf(B);
        double jcir = compute_jcir(B);
        double ps   = peak_hoop_stress_base(B);
        double mcfr = compute_mcfr(B);

        if (factor == 1.00) jcf_base = jcf;

        Bv.push_back(B); jcfv.push_back(jcf);
        jcirv.push_back(jcir); psv.push_back(ps);
        mcfrv.push_back(mcfr);

        std::cout << std::setprecision(2);
        std::cout << std::setw(4)  << factor << "x"
                  << std::setw(9)  << B * 100.0 << "%"
                  << std::setw(9)  << std::setprecision(1)
                  << res.total_coffer_mass / 1000.0 << "t"
                  << std::setw(10) << std::setprecision(3) << jcf
                  << std::setw(10) << std::setprecision(1)
                  << jcir * 100.0 << "%"
                  << std::setw(12) << std::setprecision(1)
                  << ps / 1e3
                  << std::setw(8)  << std::setprecision(1)
                  << mcfr * 100.0 << "%";

        if (factor == 1.00)
            std::cout << "  ← 基准\n";
        else
            std::cout << "\n";
    }

    std::cout << std::string(78, '-') << "\n\n";
    std::cout << "JCF  = 拱脚水平推力 / 鼓座约束能力 → 交界处裂缝风险\n";
    std::cout << "JCIR = (JCF_0 - JCF_B)/JCF_0 = B → 减重直接降低交界风险\n";
    std::cout << "MCFR = 底部拉应力降低比例 → 收缩占主导, 减重影响有限\n";
    std::cout << "\n结论: 藻井减重对交界处裂缝有直接缓解作用 (JCIR=B),\n";
    std::cout << "      对子午向裂缝影响较弱 (收缩主导)。\n\n";

    std::vector<double> fv(factors, factors + 5);
    export_arch_scan("arch_scan.csv", fv, Bv, jcfv, jcirv, psv, mcfrv);
    std::cout << "[导出] arch_scan.csv\n";

    return 0;
}
