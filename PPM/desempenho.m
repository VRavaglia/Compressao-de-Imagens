t = [6.55+5.4 6.65+5.56 7.4+6.23 8.13+6.94 9.75+7.89 12.77+9.73 16.28+12.74 20.88+17.26 43.02+29.54];
c = [1 1.9 2.56 3.01 3.62 4.13 4.4 4.48 4.48];
ctx = -1:7;

yyaxis left
plot(ctx, t, 'LineWidth', 2)
xlabel('Contexto')
ylabel('Tempo (s)')
grid on;

title("Comparação Entre Contextos")
hold on
yyaxis right
plot(ctx, c, 'LineWidth', 2);
ylabel('Compressão')