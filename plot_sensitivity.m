% plot_sensitivity.m
% 藻井深度敏感性分析 —— 减重收益 vs 局部风险
% 数据来源: scandata.txt (C++ 扫描输出)
% MATLAB R2026a

clear; close all;

% ========== 数据（自动填入） ==========
factor    = [0.60,  0.80,  1.00,  1.20,  1.40];
B_percent = [4.80,  6.43,  8.08,  9.76,  11.45];
M_coffer  = [1179.0, 1581.1, 1987.8, 2399.1, 2815.0];
Rmax      = [0.049, 0.051, 0.053, 0.055, 0.057];
E_ratio   = [98.3,  127.0, 153.5, 177.8, 199.9];

% 各层风险因子
R_all = [0.037, 0.038, 0.039, 0.040, 0.041;   % A
         0.041, 0.043, 0.044, 0.046, 0.047;   % B
         0.047, 0.049, 0.051, 0.052, 0.054;   % C
         0.043, 0.045, 0.046, 0.048, 0.050;   % D
         0.049, 0.051, 0.053, 0.055, 0.057];  % E

% ========== 图1: B vs Rmax 双Y轴 ==========
figure('Position', [50, 80, 860, 480], 'Name', '减重 vs 风险');

yyaxis left;
bar(factor, B_percent, 0.45, 'FaceColor', [0.25 0.55 0.85]);
ylabel('减重比例 $B$ (\%)', 'Interpreter', 'latex', 'FontSize', 14);
ylim([0, 14]);

yyaxis right;
plot(factor, Rmax, 'rs-', 'LineWidth', 2.8, 'MarkerSize', 12, ...
    'MarkerFaceColor', 'r');
ylabel('最大风险因子 $R_{\max}$', 'Interpreter', 'latex', 'FontSize', 14);
ylim([0.04, 0.065]);

xlabel('深度因子', 'FontSize', 14);
title('藻井深度敏感性: 减重收益 vs 局部压溃风险', 'FontSize', 15);
set(gca, 'XTick', factor);
grid on; box on;
xline(1.0, 'k--', 'LineWidth', 1.8);
text(1.02, 13.5, '实际设计', 'FontSize', 11, 'Color', [0.3 0.3 0.3]);

legend({'减重比例 $B$', '风险因子 $R_{\max}$'}, ...
    'Interpreter', 'latex', 'FontSize', 12, 'Location', 'northwest');

% 标注: Rmax 极小
annotation('textarrow', [0.72, 0.82], [0.35, 0.25], ...
    'String', sprintf('R_{max} ≈ 0.05\n(仅用5%%抗压强度!)'), ...
    'FontSize', 10, 'Color', [0.6 0 0]);

saveas(gcf, 'fig_sensitivity_tradeoff.png');
fprintf('已保存: fig_sensitivity_tradeoff.png\n');

% ========== 图2: 五层风险因子 ==========
figure('Position', [80, 100, 780, 480], 'Name', '分层风险');

hold on;
colors_line = lines(5);
layer_names = {'A(底)', 'B', 'C', 'D', 'E(顶)'};
for i = 1:5
    plot(factor, R_all(i,:), 'o-', 'Color', colors_line(i,:), ...
        'LineWidth', 2.2, 'MarkerSize', 9, 'MarkerFaceColor', colors_line(i,:));
end
xline(1.0, 'k--', 'LineWidth', 1.5);
hold off;

xlabel('深度因子', 'FontSize', 14);
ylabel('风险因子 $R_i$', 'Interpreter', 'latex', 'FontSize', 14);
title('各层藻井风险因子 $R_i$ 随深度变化', 'Interpreter', 'latex', 'FontSize', 15);
legend(layer_names, 'FontSize', 11, 'Location', 'northwest');
set(gca, 'XTick', factor);
grid on; box on;

saveas(gcf, 'fig_sensitivity_layers.png');
fprintf('已保存: fig_sensitivity_layers.png\n');

% ========== 图3: 综合效率 E = B/Rmax ==========
figure('Position', [100, 120, 680, 440], 'Name', '综合效率');

plot(factor, E_ratio, 'bs-', 'LineWidth', 2.8, 'MarkerSize', 12, ...
    'MarkerFaceColor', 'b');
hold on;
xline(1.0, 'k--', 'LineWidth', 1.5);
text(1.02, E_ratio(3)+3, sprintf('实际设计\nE = %.0f', E_ratio(3)), ...
    'FontSize', 11, 'Color', [0.3 0.3 0.3]);

% 标注: 效率持续上升
text(1.15, E_ratio(5)+3, '效率单调递增 →', ...
    'FontSize', 10, 'FontWeight', 'bold', 'Color', [0 0 0.6]);
hold off;

xlabel('深度因子', 'FontSize', 14);
ylabel('综合效率 $E = B / R_{\max}$', 'Interpreter', 'latex', 'FontSize', 14);
title('藻井深度综合效率（效率持续递增 → 力学未达最优）', 'FontSize', 14);
set(gca, 'XTick', factor);
grid on; box on;

saveas(gcf, 'fig_sensitivity_efficiency.png');
fprintf('已保存: fig_sensitivity_efficiency.png\n');

% ========== 终端汇总 ==========
fprintf('\n========== 敏感性分析汇总 ==========\n');
fprintf('因子    减重%%    减重吨    Rmax   效率E\n');
fprintf('──────────────────────────────────\n');
for i = 1:length(factor)
    marker = '';
    if factor(i) == 1.0
        marker = '  ← 实际设计';
    end
    fprintf('%.2fx   %5.1f%%   %6.0ft   %.3f   %6.1f%s\n', ...
        factor(i), B_percent(i), M_coffer(i), Rmax(i), E_ratio(i), marker);
end
fprintf('==================================\n');
fprintf('\n关键发现:\n');
fprintf('  · Rmax 始终由 E 层（最上层）控制\n');
fprintf('  · Rmax ≈ 0.05 → 仅用到抗压强度的 ~5%%\n');
fprintf('  · 效率单调递增 → 力学未达最优\n');
fprintf('  · 罗马人的深度选择可能由视觉/施工约束决定\n');
