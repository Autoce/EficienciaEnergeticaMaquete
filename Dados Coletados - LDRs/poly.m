close all;
clear;
clc;

polynum = 7;
LDR = importdata();
P_Coeff = [];
Figure = figure('visible','off');
for i = 1:4
    %% Aproximação dos pontos com funções
    
    efit = fit(LDR{i}.MediaADC, LDR{i}.LuxMedio, 'exp1'); %Exponencial 
    ssfit = fit(LDR{i}.MediaADC, LDR{i}.LuxMedio, 'smoothingspline'); %Smoothing Spline
    
    %% Geração de novos pontos usando a aproximação por Smoothing Spline.
    % Isso é necessário pois os dados obtidos experimentalmente causam
    % fenômenos de Runge com aproximações polinômiais
    
    XSpline = (0:ceil(LDR{i}.MediaADC(end)))';
    YSpline = feval(ssfit, XSpline);
    
    %% Com os pontos novos, gera-se uma aproximação polinomial
    pfit = polyfit(XSpline/1000, YSpline, polynum);
    P_Coeff = [P_Coeff; pfit];
    XPoly = XSpline/1000;
    YPoly = polyval(pfit, XPoly);
    
    subplot(2, 2, i);
    plot(LDR{i}.MediaADC, LDR{i}.LuxMedio, 'x', 'Color', 'black');
    hold on
    plot(XSpline, YPoly, '--', 'Color', '#4b0082');
    plot(efit, 'r-.');
    legend('Experimental', 'Polynomial', 'Exponential');
    
    xlabel("ADC In");
    ylabel("Lx");
    title("LDR"+(i-1)+ " - Lx/ADC");
    
    xlim([0 XSpline(end)]);
end
clearvars -except LDR P_Coeff Figure
set(Figure, 'Visible', 'on');