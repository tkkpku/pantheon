@echo off
cd /d "C:\Users\tan83\Desktop\pantheon"

echo === 1. 删除废弃文件 ===
git rm k_invariance_analysis.md deprecated_fig_math_prep.txt 2>nul

echo === 2. 添加所有改动 ===
git add -A

echo === 3. 提交 v3 ===
git commit -m "v3: 新增k不变性验证+Simpson收敛性分析+径向修正因子详解;系统化文件命名"

echo === 4. 推送到 GitHub ===
git push origin main

echo.
echo === ✅ v3 推送完成！ ===
pause
