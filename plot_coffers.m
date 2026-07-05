% plot_coffers.m
% 藻井尺寸可视化 + 减重分析
% 数据来源: pantheondata.txt (C++ 输出)
% MATLAB R2026a

clear; close all;

% ========== 实测数据（自动填入） ==========
layer_names = {'A(最下)', 'B', 'C', 'D', 'E(最上)'};
arc_h    = [4.08, 3.79, 3.52, 3.05, 2.39];   % 弧高 m
w_bot    = [3.88, 3.74, 3.39, 2.92, 2.38];   % 底宽 m
w_top    = [3.77, 3.48, 3.02, 2.50, 2.02];   % 顶宽 m
depth    = [0.67, 0.65, 0.61, 0.54, 0.48];   % 深度 m
V_single = [10.78, 9.16, 7.08, 4.57, 2.58];  % 单个体积 m³
V_layer  = [301.84, 256.55, 198.17, 128.09, 72.22];  % 层体积 m³
M_layer  = [633.86, 538.76, 416.16, 269.00, 130.00]; % 层质量 吨

M_total  = 24587.5;      % 无藻井穹顶总质量 吨
M_coffer = 1987.79;      % 藻井挖去总质量 吨
B_pct    = 8.085;        % 减重比例 %

% 四级台阶深度 (Table 3)
step_depth = [0.23 0.19 0.13 0.12;
              0.22 0.18 0.13 0.12;
              0.21 0.18 0.12 0.10;
              0.20 0.16 0.11 0.07;
              0.19 0.15 0.14  NaN];

fprintf('========== 藻井减重汇总 ==========\n');
fprintf('无藻井穹顶总质量: %.0f 吨\n', M_total);
fprintf('藻井挖去总质量:   %.0f 吨\n', M_coffer);
fprintf('减重比例:         %.2f %%\n', B_pct);
fprintf('藻井挖去总体积:   %.2f m³\n', sum(V_layer));
fprintf('==================================\n');

% ========== 图1: 藻井尺寸 + 台阶 + 减重 ==========
figure('Position', [50, 80, 1100, 420], 'Name', '藻井几何分析');

% --- (a) 藻井尺寸变化 ---
subplot(1,4,1);
x = 1:5;
hold on;
plot(x, w_bot, 'bo-', 'LineWidth', 2, 'MarkerFaceColor', 'b', 'MarkerSize', 8);
plot(x, w_top, 'rs-', 'LineWidth', 2, 'MarkerFaceColor', 'r', 'MarkerSize', 8);
plot(x, arc_h, 'g^-', 'LineWidth', 2, 'MarkerFaceColor', 'g', 'MarkerSize', 8);
plot(x, depth, 'md-', 'LineWidth', 2, 'MarkerFaceColor', 'm', 'MarkerSize', 8);
hold off;
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('藻井层', 'FontSize', 11);
ylabel('尺寸 (m)', 'FontSize', 11);
title('藻井尺寸逐层变化', 'FontSize', 12);
legend({'底宽', '顶宽', '弧高', '深度'}, 'FontSize', 9, 'Location', 'northeast');
grid on; box on;

% --- (b) 四级台阶深度堆积 ---
subplot(1,4,2);
h_bar = bar(step_depth, 'stacked', 'BarWidth', 0.7);
colormap(gca, parula(4));
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('藻井层', 'FontSize', 11);
ylabel('深度 (m)', 'FontSize', 11);
title('四级阶梯深度分布', 'FontSize', 12);
legend({'第1级', '第2级', '第3级', '第4级'}, ...
    'FontSize', 8, 'Location', 'northeast');
grid on; box on;

% --- (c) 各层减重贡献 ---
subplot(1,4,3);
bar_colors = lines(5);
h_bar2 = bar(M_layer, 'FaceColor', 'flat');
for i = 1:5
    h_bar2.CData(i,:) = bar_colors(i,:);
end
set(gca, 'XTick', 1:5, 'XTickLabel', layer_names);
xlabel('藻井层', 'FontSize', 11);
ylabel('质量 (吨)', 'FontSize', 11);
title('每层藻井挖去质量', 'FontSize', 12);
for i = 1:5
    text(i, M_layer(i) + 10, sprintf('%.0f', M_layer(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 9, 'FontWeight', 'bold');
end
grid on; box on;

% --- (d) 减重比例饼图 ---
subplot(1,4,4);
pie_data = [M_coffer, M_total - M_coffer];
h_pie = pie(pie_data, {sprintf('藻井\n%.0f吨', M_coffer), ...
                       sprintf('穹顶保留\n%.0f吨', M_total-M_coffer)});
for i = 2:2:length(h_pie)
    h_pie(i).FontSize = 11;
    h_pie(i).FontWeight = 'bold';
end
title(sprintf('减重: %.1f%%', B_pct), 'FontSize', 13);

saveas(gcf, 'fig_coffer_geometry.png');
fprintf('已保存: fig_coffer_geometry.png\n');
