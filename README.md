# pantheon

万神殿穹顶藻井的减重效益与裂缝影响分析。基于激光扫描实测几何，计算 140 个藻井挖去的混凝土质量，并在穹顶开裂后的拱券模型下，分析减重对两类裂缝驱动力的影响。

纯 C++ 计算，无第三方数值库；绘图用 MATLAB R2026a。推导细节见 [math_preparation.md](math_preparation.md)。

## 结论

| 量 | 值 |
| --- | --- |
| 穹顶总质量 | 24,587.5 t |
| 藻井挖去质量 | 1,987.8 t |
| 减重比例 B | 8.09% |
| 交界处裂缝改善率 JCIR | 等于 B（力学恒等式） |
| 子午向裂缝改善率 MCFR | 约 5.8% |
| 单拱自重 | 8.6 降至 7.9 MN（−8.1%） |
| 拱脚推力 | 3.5 降至 3.2 MN（−8.1%） |
| 收缩占总驱动力 | 约 29%，自重占 71.3% |

JCIR 等于 B 的原因：交界处裂缝由拱脚推力驱动，推力与自重成正比，每减重 1% 驱动力降低 1%。MCFR 低于 B 的原因：子午向裂缝由混凝土收缩主导，收缩是材料属性，不受减重影响。

抗拉强度取 f_t = 80 kPa（Masi 2018）。据此，裂缝是设计状态而非缺陷：穹顶必然开裂，开裂后转为 28 个楔形拱阵列，进入无筋混凝土擅长的受压工作状态。

## 依赖

- g++，编译 C++11
- MATLAB R2026a，仅绘图，可跳过

```bash
g++ -std=c++11 -O2 main.cpp pantheon.cpp -o pantheon.exe
g++ -std=c++11 -O2 main_scan.cpp pantheon.cpp -o scan.exe
```

## 用法

```bash
./pantheon.exe      # 输出 stress_profile.csv（201 行）与 coffer_details.csv
./scan.exe          # 深度 0.6 至 1.4 倍扫描，输出 arch_scan.csv
```

MATLAB 依次执行 `plot_stress`、`plot_coffers`、`plot_arch_model`、`plot_sensitivity`、`plot_dome_profile`，生成 fig01 至 fig07 与 `output_summary.txt`、`scan_summary.txt`。

## 输入数据

几何取自 Aliberti 与 Alonso-Rodríguez (2017) 的激光扫描，经摄影测量双重验证。

| 项 | 值 |
| --- | --- |
| 穹顶半径 | 22.03 m |
| Oculus 半径 | 4.45 m |
| 径向等分 | 28 份，平均角度偏差 0.36° |
| 壳厚 | 底部 5.9 m 收至顶部 1.2 m |
| 鼓座墙厚 | 6 m |
| 藻井数 | 140（5 圈 × 28） |

藻井五层尺寸，单位 m，每层 28 个：

| 层 | 底宽 | 顶宽 | 弧高 | 深度合计 |
| --- | --- | --- | --- | --- |
| A | 3.88 | 3.77 | 4.08 | 0.67 |
| B | 3.74 | 3.48 | 3.79 | 0.65 |
| C | 3.39 | 3.02 | 3.52 | 0.61 |
| D | 2.92 | 2.50 | 3.05 | 0.54 |
| E | 2.38 | 2.02 | 2.39 | 0.48 |

材料：抗拉强度 80 kPa，弹性模量 14.8 GPa，泊松比 0.2，密度自底部的石灰华约 2200 kg/m³ 过渡到顶部浮石约 1350 kg/m³。旧文献给出的抗拉强度为 1.0 至 1.2 MPa。

## 方法

- 棱台体积 V = d/3 × (S_t + S_b + √(S_t S_b))，径向修正因子 s_c = (R + d)/R
- 质量积分 M = 2πR² ∫ ρ(φ) t(φ) sinφ dφ，Simpson，n = 200
- 薄膜环向应力 N_θ = −gR(cosφ − 1/(1 + cosφ))，环向转压点 cosφ_c = (√5 − 1)/2，自顶 51.8°
- 推力系数 k ≈ (1 − sinφ_oc)/(2cosφ_oc) ≈ 0.407；三维数值标定 k₀ = 0.321、k_B = 0.317
- 底部有效环向拉应力约 635 kPa，其中自重项 σ_θ0 约 453 kPa，收缩项 σ_sh 约 182 kPa
- MCFR = B × σ_θ0/(σ_sh + σ_θ0) ≈ 8.09% × 453/635 ≈ 5.8%

## 验证

| 项 | 结果 |
| --- | --- |
| Simpson 收敛 | n = 200 相对误差 0.0007%，对 B 影响不超过 0.00006 个百分点 |
| 棱台公式 | 退化为柱体、锥体两个极限情形均通过；A 层手算 10.78 m³ 与 C++ 一致 |
| 推力系数不变性 | k 绝对值差约 21%，但随深度变化的相对幅度仅 −1.1%，JCIR 结论稳定 |
| 与 FEM 对照 | Mark 与 Hutchinson 峰值约 128 kPa，薄膜解 453 kPa，高估约 3.5 倍，趋势一致 |

主导误差来自几何扫描，约 1%，远大于数值积分误差。

## 已知限制

- 薄膜解只用于判断形状是否成立，不用于定量校核，环向拉应力高估约 3.5 倍。
- σ_sh = 182 kPa 是按实测裂缝位置反标定的值。若按 Eε_sh 计算约 5.9 MPa，两者相差约 30 倍。
- 推力系数 k 的绝对值依赖简化推力线假设，只保证随深度的相对变化稳定。
- 减重比例 B 的误差主要来自几何扫描，约 1%。

## 与另一仓库的参数差异

本仓库与 `pantheon-3d-fem` 的部分参数不同，尚未裁决：

| 项 | 本仓库 | pantheon-3d-fem |
| --- | --- | --- |
| 内径 | 43.3 m（与自身 R = 22.03 m 不自洽） | 44.06 m |
| 顶部厚度 | 1.2 m | 1.5 m |
| 密度分层 | 4 层，跳变在 12°、25°、40° | 3 档，1350 / 1500 / 1600 |
| Oculus | 半径 4.45 m | 直径 8.95 m |

厚度与密度直接进入质量积分，引用结论前需先定一个取值。

## 文件

| 文件 | 内容 |
| --- | --- |
| `main.cpp`、`main_scan.cpp` | 主算与扫描入口 |
| `pantheon.cpp`、`pantheon.h` | 几何、积分、拱券模型 |
| `*.m` | 5 个绘图脚本 |
| `math_preparation.md` | Simpson 积分、棱台公式、减重比例定义的推导 |
| `fig01` 至 `fig07` | 应力剖面、藻井数据、拱券模型、敏感性、穹顶剖面、k 不变性、收敛 |

## 参考文献

1. Aliberti, L., Alonso-Rodríguez, M.Á. (2017). Nexus Network Journal 19: 465–489.
2. Masi, F., Stefanou, I., Vannucci, P. (2018). Engineering Failure Analysis 92: 587–596.
3. Mark, R., Hutchinson, P. (1986). Art Bulletin 68(1): 24–34.
4. Moore, D. (1995). The Roman Pantheon.
5. Waddell, G. (2008). Creating the Pantheon.
6. Fine Licht, K. de (1968). The Rotunda in Rome.
7. Heyman, J. (1966). The Stone Skeleton.
8. Calladine, C.R. (1983). Theory of Shell Structures.
9. Terenzio, A. (1934). pp. 280–285.

仓库未附许可证文件。代码引用请注明出处。
