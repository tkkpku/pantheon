#include "pantheon.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

int main() {
    std::cout << "==============================================\n";
    std::cout << "  万神殿穹顶藻井 — 拱券模型 & 两类裂缝分析\n";
    std::cout << "  R = 22.03 m,  f_t = " << FT_DOME/1e3 << " kPa\n";
    std::cout << "==============================================\n\n";

    // ---- 藻井几何参数 ----
    // 数据来源: Aliberti & Alonso-Rodríguez (2017) Table 2 & 3
    // band: 藻井行间环向间距（实测平均）
    double band = 0.84;
    double h[5]      = {4.08, 3.79, 3.52, 3.05, 2.39};  // 弧高 (m)
    double w_bot[5]  = {3.88, 3.74, 3.39, 2.92, 2.38};  // 底宽 (m)
    double w_top[5]  = {3.77, 3.48, 3.02, 2.50, 2.02};  // 顶宽 (m)
    double depths[5] = {0.67, 0.65, 0.61, 0.54, 0.48};  // 深度 (m)
    const char* names[5] = {"A(底)", "B", "C", "D", "E(顶)"};

    // ---- 计算各层中心纬度 ----
    double phi_oculus = asin(R_OCULUS / R);
    double cum = band / 2.0;
    double arc[5];
    for (int i = 0; i < 5; i++) { arc[i] = cum + h[i]/2.0; cum += h[i] + band; }

    // ---- 构建藻井层数据 ----
    std::vector<CofferLayer> layers;
    for (int i = 0; i < 5; i++) {
        CofferLayer L;
        L.name = names[i]; L.phi_center = PI/2.0 - arc[i]/R;
        L.arc_height = h[i]; L.w_bottom = w_bot[i];
        L.w_top = w_top[i]; L.depth = depths[i]; L.count = 28;
        layers.push_back(L);
    }

    std::cout << "正在计算...\n\n";
    CofferResult res = compute_coffers(layers);
    double B = res.mass_reduction_ratio;

    // ---- 拱券分析 ----
    // 分别计算有/无藻井时的拱券力学量
    double W0  = arch_self_weight(0.0);     // 无藻井单拱自重
    double WB  = arch_self_weight(B);       // 有藻井单拱自重
    double H0  = arch_horizontal_thrust(0.0); // 无藻井拱脚推力
    double HB  = arch_horizontal_thrust(B);   // 有藻井拱脚推力
    double jcf0 = compute_jcf(0.0);          // 无藻井交界处裂缝因子
    double jcfB = compute_jcf(B);            // 有藻井交界处裂缝因子
    double jcir = compute_jcir(B);           // JCIR = B（力学恒等式）
    double ps0  = peak_hoop_stress_base(0.0); // 无藻井底部有效拉应力
    double psB  = peak_hoop_stress_base(B);   // 有藻井底部有效拉应力
    double mcfr = compute_mcfr(B);           // 子午向裂缝驱动力降低率

    std::cout << std::fixed;
    std::cout << "========== 减重结果 ==========\n";
    std::cout << "无藻井穹顶总质量:   " << std::setprecision(1)
              << res.mass_no_coffers / 1000.0 << " 吨\n";
    std::cout << "藻井挖去总质量:     "
              << res.total_coffer_mass / 1000.0 << " 吨\n";
    std::cout << "减重比例 B:         " << std::setprecision(3)
              << B * 100.0 << " %\n\n";

    std::cout << "========== 拱券模型 (开裂后状态) ==========\n";
    std::cout << "楔形拱数量:          28 个\n";
    std::cout << "单拱自重 (无藻井):   " << std::setprecision(1)
              << W0 / 1e6 << " MN\n";
    std::cout << "单拱自重 (有藻井):   " << WB / 1e6 << " MN\n";
    std::cout << "自重降低:            " << std::setprecision(1)
              << (W0 - WB) / W0 * 100.0 << " %\n\n";
    std::cout << "拱脚推力 (无藻井):   " << H0 / 1e6 << " MN\n";
    std::cout << "拱脚推力 (有藻井):   " << HB / 1e6 << " MN\n";
    std::cout << "推力降低:            "
              << (H0 - HB) / H0 * 100.0 << " %\n\n";

    std::cout << "========== 交界处裂缝 (JCF) ==========\n";
    std::cout << "JCF (无藻井):        " << std::setprecision(3)
              << jcf0 << "\n";
    std::cout << "JCF (有藻井):        " << jcfB << "\n";
    std::cout << "交界处裂缝改善率 JCIR: "
              << std::setprecision(1) << jcir * 100.0 << " %\n";
    std::cout << "  (= 拱脚推力降低比例 = 减重比例 B)\n\n";

    std::cout << "========== 子午向裂缝 ==========\n";
    std::cout << "底部有效拉应力 (无藻井): " << std::setprecision(1)
              << ps0 / 1e3 << " kPa\n";
    std::cout << "底部有效拉应力 (有藻井): " << psB / 1e3 << " kPa\n";
    std::cout << "底部驱动力降低率 MCFR:   "
              << std::setprecision(1) << mcfr * 100.0 << " %\n";
    std::cout << "  (收缩占主导 → 减重影响有限)\n\n";

    std::cout << "========== 两类裂缝对比 ==========\n";
    std::cout << "              驱动力降低   藻井影响\n";
    std::cout << "交界处裂缝    " << std::setprecision(1)
              << jcir * 100.0 << "%         强 (H ∝ 自重)\n";
    std::cout << "子午向裂缝    "
              << mcfr * 100.0 << "%          弱 (收缩主导)\n\n";

    std::cout << "========== 各层藻井 ==========\n";
    std::cout << "层     纬度°  单体积m³  层质量吨\n"
              << "──────────────────────────────────\n";
    for (size_t i = 0; i < layers.size(); i++) {
        double V1 = single_coffer_volume_prism(
            layers[i].phi_center, layers[i].arc_height,
            layers[i].w_bottom, layers[i].w_top, layers[i].depth);
        std::cout << std::setw(6) << std::left << layers[i].name
                  << std::setw(7) << std::right << std::setprecision(1)
                  << layers[i].phi_center * 180.0 / PI
                  << std::setw(10) << std::setprecision(2) << V1
                  << std::setw(10) << res.layer_masses[i] / 1000.0 << "\n";
    }
    std::cout << "──────────────────────────────────\n\n";

    // ---- 导出 CSV 数据供 MATLAB 绘图 ----
    export_stress_profile("stress_profile.csv", 200);
    export_layer_details("coffer_details.csv", layers, res);
    std::cout << "[导出] stress_profile.csv\n";
    std::cout << "[导出] coffer_details.csv\n";

    return 0;
}
