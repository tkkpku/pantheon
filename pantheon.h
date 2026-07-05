#ifndef PANTHEON_H
#define PANTHEON_H

#include <cmath>
#include <vector>
#include <string>

// ============================================================
// 公共参数 —— 基于 Aliberti & Alonso-Rodríguez (2017)
// ============================================================
const double R        = 22.03;   // 球面平均半径 (m)，激光扫描实测
const double R_OCULUS = 4.45;    // Oculus 半径 (m)
const double T_BASE   = 5.9;     // 穹顶底部厚度 (m)
const double T_OCULUS = 1.2;     // Oculus 边缘厚度 (m)
const double PI       = 3.14159265358979323846;
const double G        = 9.8;     // 重力加速度 (m/s²)

// 球心低于檐口平面
const double CENTER_OFFSET = 0.37;  // (m)

// ============================================================
// 密度函数（四层梯度）
// ============================================================
double density(double phi);

// ============================================================
// 厚度函数（线性递减）
// ============================================================
double thickness(double phi);

// ============================================================
// Simpson 积分
// ============================================================
double simpson(double (*f)(double), double a, double b, int n = 200);

// ============================================================
// 无藻井穹顶总质量
// ============================================================
double total_mass_no_coffers();

// ============================================================
// 单个藻井体积 —— 平底棱台公式
// ============================================================
// 论文证实藻井内表面是平的，不是球面。
// 藻井 = 四棱台：底面（靠近外表面）> 顶面（内表面开口）
//
// phi_center : 藻井中心纬度 (rad)
// arc_height : 藻井沿经线的弧长 (m)  ——论文 Table 2 "Height"
// w_bottom   : 藻井底部宽度 (m)        ——论文 Table 2 "Bottom width"
// w_top      : 藻井顶部宽度 (m)        ——论文 Table 2 "Upper width"
// depth      : 藻井总深度 (m)          ——论文 Table 3 各级之和
double single_coffer_volume_prism(double phi_center,
    double arc_height, double w_bottom, double w_top, double depth);

// ============================================================
// 数据结构
// ============================================================
struct CofferLayer {
    std::string name;       // "A"~"E"
    double phi_center;      // 中心纬度 (rad)
    double arc_height;      // 沿经线的弧长 (m)
    double w_bottom;        // 底宽 (m)
    double w_top;           // 顶宽 (m)
    double depth;           // 总深度 (m)
    int    count;           // 28
};

struct CofferResult {
    double total_coffer_volume;
    double total_coffer_mass;
    double mass_no_coffers;
    double mass_reduction_ratio;
    std::vector<double> layer_volumes;
    std::vector<double> layer_masses;
};

CofferResult compute_coffers(const std::vector<CofferLayer>& layers);

// ============================================================
// CSV 导出
// ============================================================
void export_stress_profile(const char* filename, int n = 200);
void export_layer_details(const char* filename,
    const std::vector<CofferLayer>& layers,
    const CofferResult& result);

#endif
