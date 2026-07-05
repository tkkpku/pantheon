#include "pantheon.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    std::cout << "==============================================\n";
    std::cout << "  万神殿穹顶藻井几何建模与减重计算\n";
    std::cout << "  课题 2 — 基于 Aliberti & Alonso-Rodriguez (2017)\n";
    std::cout << "  球面半径 R = 22.03 m（激光扫描实测）\n";
    std::cout << "==============================================\n\n";

    // =====================================================
    // 藻井数据 —— 全部来自论文实测值
    // =====================================================
    //
    // 论文 Table 2：藻井尺寸（North-East quadrant, sectors 2-6 平均）
    //   层   底宽(m)  顶宽(m)  高度/弧长(m)
    //   A    3.88     3.77     4.08         ← 最下层，靠近鼓座
    //   B    3.74     3.48     3.79
    //   C    3.39     3.02     3.52
    //   D    2.92     2.50     3.05
    //   E    2.38     2.02     2.39         ← 最上层，靠近 Oculus
    //
    // 论文 Table 3：各级阶梯深度 → 总深度 (m)
    //   A: 0.23+0.19+0.13+0.12 = 0.67
    //   B: 0.22+0.18+0.13+0.12 = 0.65
    //   C: 0.21+0.18+0.12+0.10 = 0.61
    //   D: 0.20+0.16+0.11+0.07 = 0.54
    //   E: 0.19+0.15+0.14      = 0.48
    //
    // 环向水平带（层间分隔）: 平均 0.84 m
    // 经线肋宽: 底部 1.02 m → 顶部 0.67 m
    //
    // 藻井内表面是平的（论文原文："tend to be flat"）

    // --- 计算各层藻井中心的纬度 phi ---
    // 球心在檐口平面以下 0.37 m
    // 檐口处 phi ≈ π/2
    // 从檐口向上，依次排列：水平带 → 藻井A → 带 → 藻井B → ...
    //
    // 藻井 A 底部距檐口 ≈ 水平带宽度的一半（约 0.42 m 弧长）
    // 简化：将各层藻井中线按弧长均匀分布

    double phi_oculus = asin(R_OCULUS / R);  // ≈ 11.66°
    double phi_cornice = PI / 2.0;            // 90° (檐口)

    // 水平带 0.84 m，藻井高度依次为 4.08, 3.79, 3.52, 3.05, 2.39 m
    double band = 0.84;  // m (弧长)
    double h[5] = {4.08, 3.79, 3.52, 3.05, 2.39};  // A~E

    // 从檐口向上的累积弧长（到每层藻井的中心）
    double arc_from_cornice[5];
    double cum = band / 2.0;  // 第一个半带
    for (int i = 0; i < 5; i++) {
        arc_from_cornice[i] = cum + h[i] / 2.0;
        cum += h[i] + band;
    }

    // 从檐口(phi=90°)向上减去弧长/R 得到 phi
    std::vector<CofferLayer> layers;
    const char* names[5] = {"A(最下)", "B", "C", "D", "E(最上)"};
    double w_bot[5] = {3.88, 3.74, 3.39, 2.92, 2.38};
    double w_top[5] = {3.77, 3.48, 3.02, 2.50, 2.02};
    double depths[5] = {0.67, 0.65, 0.61, 0.54, 0.48};

    for (int i = 0; i < 5; i++) {
        CofferLayer L;
        L.name       = names[i];
        L.phi_center = PI / 2.0 - arc_from_cornice[i] / R;
        L.arc_height = h[i];
        L.w_bottom   = w_bot[i];
        L.w_top      = w_top[i];
        L.depth      = depths[i];
        L.count      = 28;
        layers.push_back(L);
    }

    // =====================================================
    // 计算
    // =====================================================
    std::cout << "正在计算...\n\n";
    CofferResult res = compute_coffers(layers);

    // =====================================================
    // 输出
    // =====================================================
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "========== 计算结果 ==========\n";
    std::cout << "无藻井穹顶总质量:   "
              << res.mass_no_coffers / 1000.0 << " 吨\n";
    std::cout << "藻井挖去总质量:     "
              << res.total_coffer_mass / 1000.0 << " 吨\n";
    std::cout << "减重比例:           "
              << res.mass_reduction_ratio * 100.0 << " %\n\n";

    std::cout << "各层藻井详情:\n";
    std::cout << "层     纬度°   弧高m   底宽m  顶宽m   深度m  ";
    std::cout << "单个体积m³  层体积m³  层质量吨\n";
    std::cout << "──────────────────────────────────────────────";
    std::cout << "──────────────────\n";
    for (size_t i = 0; i < layers.size(); i++) {
        const CofferLayer& L = layers[i];
        double V1 = single_coffer_volume_prism(
            L.phi_center, L.arc_height, L.w_bottom, L.w_top, L.depth);

        std::cout << std::setw(8)  << std::left  << L.name
                  << std::setw(7)  << std::right << std::setprecision(1)
                  << L.phi_center * 180.0 / PI
                  << std::setw(8)  << std::setprecision(2) << L.arc_height
                  << std::setw(7)  << L.w_bottom
                  << std::setw(7)  << L.w_top
                  << std::setw(7)  << L.depth
                  << std::setw(10) << std::setprecision(2) << V1
                  << std::setw(10) << res.layer_volumes[i]
                  << std::setw(10) << res.layer_masses[i] / 1000.0
                  << "\n";
    }
    std::cout << "──────────────────────────────────────────────";
    std::cout << "──────────────────\n";
    std::cout << std::setw(8)  << std::left  << "合计"
              << std::setw(7)  << " "
              << std::setw(8)  << " "
              << std::setw(7)  << " "
              << std::setw(7)  << " "
              << std::setw(7)  << " "
              << std::setw(10) << " "
              << std::setw(10) << res.total_coffer_volume
              << std::setw(10) << res.total_coffer_mass / 1000.0
              << "\n\n";

    // =====================================================
    // 导出 CSV
    // =====================================================
    export_stress_profile("stress_profile.csv", 200);
    export_layer_details("coffer_details.csv", layers, res);

    std::cout << "========== 导出文件 ==========\n";
    std::cout << "stress_profile.csv  → MATLAB 画应力分布图\n";
    std::cout << "coffer_details.csv  → MATLAB 画藻井对比图\n";
    std::cout << "==============================\n";

    return 0;
}
