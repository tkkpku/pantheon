% plot_sensitivity.m (v4)
% Depth factor scan: JCIR vs MCFR
% Data: arch_scan.csv
% MATLAB R2026a

clear; close all;

CS = readtable('arch_scan.csv');
factor      = CS.factor;
B_pct       = CS.B_percent;
JCF         = CS.JCF;
JCIR        = CS.JCIR_percent;
peak_stress = CS.peak_stress_kPa;
MCFR        = CS.MCFR_percent;

fprintf('========== Depth Sensitivity Summary ==========\n');
fprintf('Factor   B%%      JCF      JCIR%%   MCFR%%\n');
fprintf('----------------------------------------------\n');
for i = 1:length(factor)
    fprintf('%.2fx   %5.1f%%  %6.3f   %5.1f%%   %5.1f%%', ...
        factor(i), B_pct(i), JCF(i), JCIR(i), MCFR(i));
    if factor(i) == 1.0
        fprintf('  <- actual\n');
    else
        fprintf('\n');
    end
end
fprintf('============================================\n');

figure('Position', [50, 80, 850, 480]);

yyaxis left;
h1 = bar(factor, JCIR, 0.5, 'FaceColor', [0.2 0.6 0.2]);
ylabel('JCIR - Junction Crack Improvement (%)', 'FontSize', 13);
ylim([0, 16]);

yyaxis right;
h2 = plot(factor, MCFR, 'bs-', 'LineWidth', 2.5, 'MarkerSize', 10, ...
    'MarkerFaceColor', 'b');
ylabel('MCFR - Meridional Crack Force Reduction (%)', 'FontSize', 13);
ylim([0, 16]);

xlabel('Depth Factor', 'FontSize', 14);
title('Coffer Depth -> Two Crack Types Response', 'FontSize', 15);
set(gca, 'XTick', factor);
grid on; box on;
xline(1.0, 'k--', 'LineWidth', 1.5);

legend([h1, h2], {'JCIR (Junction Crack)', 'MCFR (Meridional Crack)'}, ...
    'FontSize', 11, 'Location', 'northwest');
text(1.15, JCIR(3)+1, 'JCIR == B (identity)', ...
    'FontSize', 10, 'Color', [0 0.4 0], 'FontWeight', 'bold');
text(1.15, MCFR(3)+1, 'Shrinkage-dominant', ...
    'FontSize', 10, 'Color', [0 0 0.6]);

saveas(gcf, 'figs/fig04a_sensitivity.png');
fprintf('Saved: figs/fig04a_sensitivity.png\n');

figure('Position', [80, 100, 650, 420]);
plot(factor, JCF, 'ro-', 'LineWidth', 2.5, 'MarkerSize', 10, 'MarkerFaceColor', 'r');
xlabel('Depth Factor', 'FontSize', 14);
ylabel('JCF - Junction Crack Factor', 'FontSize', 14);
title('Deeper Coffers -> Lower JCF -> Safer', 'FontSize', 15);
set(gca, 'XTick', factor);
grid on; box on;
xline(1.0, 'k--', 'LineWidth', 1.5);
text(1.02, JCF(3)+0.001, 'Actual design', 'FontSize', 11, 'Color', [0.3 0.3 0.3]);
for i = 1:length(factor)
    text(factor(i), JCF(i) + 0.001, sprintf('%.3f', JCF(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 9);
end
saveas(gcf, 'figs/fig04b_jcf_decrease.png');
fprintf('Saved: figs/fig04b_jcf_decrease.png\n');

figure('Position', [100, 120, 650, 420]);
plot(factor, peak_stress, 'mo-', 'LineWidth', 2.5, 'MarkerSize', 10, 'MarkerFaceColor', 'm');
xlabel('Depth Factor', 'FontSize', 14);
ylabel('Peak hoop stress at base (kPa)', 'FontSize', 14);
title('Deeper Coffers -> Lower Peak Tensile Stress', 'FontSize', 15);
set(gca, 'XTick', factor);
grid on; box on;
xline(1.0, 'k--', 'LineWidth', 1.5);
for i = 1:length(factor)
    text(factor(i), peak_stress(i) + 2, sprintf('%.0f', peak_stress(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 9);
end
saveas(gcf, 'figs/fig04c_peak_stress.png');
fprintf('Saved: figs/fig04c_peak_stress.png\n');
