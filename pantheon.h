#ifndef PANTHEON_H
#define PANTHEON_H

#include <cmath>
#include <vector>
#include <string>

const double R        = 22.03;
const double R_OCULUS = 4.45;
const double T_BASE   = 5.9;
const double T_OCULUS = 1.2;
const double PI       = 3.14159265358979323846;
const double G        = 9.8;

// Masi et al. (2018) Table 1
const double FT_DOME = 0.08e6;

// 收缩等效拉应力
const double SIGMA_SHRINKAGE = 0.182e6;

// 鼓座环向约束能力 (N) — 标称值，用于归一化 JCF
const double DRUM_HOOP_CAPACITY = 5.0e7;

double density(double phi);
double thickness(double phi);
double simpson(double (*f)(double), double a, double b, int n = 200);

// 薄膜应力 (仅供对比)
double N_phi_val(double phi);
double N_theta_val(double phi);
double sigma_theta(double phi);

// 质量计算
double total_mass_no_coffers();
double single_coffer_volume_prism(double phi_center,
    double ah, double wb, double wt, double d);

// ============================================================
// 拱券模型 — 开裂后状态
// ============================================================

// 单个楔形拱的总自重 (N)
double arch_self_weight(double weight_reduction);

// 拱脚水平推力 (N)
// H ∝ W_arch，比例系数 ~0.4（推力线几何）
double arch_horizontal_thrust(double weight_reduction);

// ============================================================
// 交界处裂缝因子 (JCF)
// ============================================================
// JCF = H / DRUM_HOOP_CAPACITY
// JCF > 1 → 交界处可能开裂
// 减重 B% → H 降低 B% → JCF 降低 B%
double compute_jcf(double weight_reduction);

// 交界处裂缝改善率
double compute_jcir(double weight_reduction);

// ============================================================
// 子午向裂缝分析
// ============================================================
// 底部峰值有效拉应力 (含收缩) — 控制裂缝起始
double peak_hoop_stress_base(double weight_reduction);

// 子午向裂缝底部驱动力降低率
double compute_mcfr(double weight_reduction);

// ============================================================
// 数据结构
// ============================================================
struct CofferLayer {
    std::string name;
    double phi_center, arc_height, w_bottom, w_top, depth;
    int count;
};

struct CofferResult {
    double total_coffer_volume, total_coffer_mass;
    double mass_no_coffers, mass_reduction_ratio;
    std::vector<double> layer_volumes, layer_masses;
};

CofferResult compute_coffers(const std::vector<CofferLayer>& layers);

// ============================================================
// CSV 导出
// ============================================================
void export_stress_profile(const char* fn, int n = 200);
void export_layer_details(const char* fn,
    const std::vector<CofferLayer>& layers, const CofferResult& res);
void export_arch_scan(const char* fn,
    const std::vector<double>& factors,
    const std::vector<double>& Bv,
    const std::vector<double>& jcfv,
    const std::vector<double>& jcirv,
    const std::vector<double>& peak_stress,
    const std::vector<double>& mcfrv);

#endif
