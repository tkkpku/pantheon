% plot_stress.m (v4)
% Hoop stress + two crack types comparison
% Data: stress_profile.csv
% MATLAB R2026a

clear; close all;

T = readtable('stress_profile.csv');
phi_horiz  = T.phi_horiz_deg;
sigma_film = T.sigma_theta_kPa;
sigma_eff  = T.sigma_eff_kPa;
ft_ref     = T.ft_ref_kPa(1);

B_pct    = 8.085;
JCIR_pct = 8.1;
MCFR_pct = 5.8;

fprintf('ft = %.0f kPa\n', ft_ref);

figure('Position', [50, 100, 1000, 420]);

subplot(1,2,1);
hold on;
plot(phi_horiz, sigma_film, 'b-', 'LineWidth', 2);
plot(phi_horiz, sigma_eff,  'r-', 'LineWidth', 2.2);
yline(ft_ref, 'k--', 'LineWidth', 1.3);
yline(0, 'k-', 'LineWidth', 0.6);
xline(57, 'k:', 'LineWidth', 1.2);
hold off;
xlabel('Latitude (deg, from horizontal plane)', 'FontSize', 12);
ylabel('Hoop stress (kPa)', 'FontSize', 13);
title('Hoop Stress Distribution', 'FontSize', 14);
legend({'\sigma_\theta (membrane)', '\sigma_\theta+\sigma_{sh} (effective)', ...
    sprintf('f_t = %.0f kPa', ft_ref)}, 'FontSize', 10, 'Location', 'northeast');
grid on; box on;

subplot(1,2,2);
cats = {'Junction Crack', 'Meridional Crack'};
vals = [JCIR_pct, MCFR_pct];
h_bar = bar(vals, 0.5);
h_bar.FaceColor = 'flat';
h_bar.CData(1,:) = [0.2 0.6 0.2];
h_bar.CData(2,:) = [0.6 0.6 0.6];
set(gca, 'XTickLabel', cats, 'FontSize', 12);
ylabel('Driving force reduction (%)', 'FontSize', 13);
title(sprintf('Effect of %.1f%% Mass Reduction on Two Crack Types', B_pct), 'FontSize', 14);
for i = 1:2
    text(i, vals(i) + 0.3, sprintf('%.1f%%', vals(i)), ...
        'HorizontalAlignment', 'center', 'FontSize', 13, 'FontWeight', 'bold');
end
text(1, vals(1)/2, 'Thrust reduction = mass reduction', ...
    'FontSize', 9, 'Color', 'white', 'FontWeight', 'bold', 'HorizontalAlignment', 'center');
text(2, vals(2)/2, 'Shrinkage-dominant,\nweaker response', ...
    'FontSize', 9, 'Color', [0.3 0.3 0.3], 'FontWeight', 'bold', 'HorizontalAlignment', 'center');
grid on; box on;
ylim([0, 12]);

saveas(gcf, 'fig_stress_two_cracks.png');
fprintf('Saved: fig_stress_two_cracks.png\n');
