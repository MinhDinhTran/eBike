close all;
clear all;
clc;
% % load('Data');
% % load('D');

load('HAE');
load('HAE2');
D.Duty(588:end) = 60;

T = D.T;
Vbat = D.Vbat;
Current = D.Current;
RPM = D.Speed;
Duty = D.Duty;

load('HAE_2');
load('HAE_22');
D.Duty(1340:end) = 20;

T = cat(2, T, D.T);
Vbat = cat(2, Vbat, D.Vbat);
Current = cat(2, Current, D.Current);
RPM = cat(2, RPM, D.Speed);
Duty = cat(2, Duty, D.Duty);





W = -((Current.*0.0217)-44.4+0.6).*(Vbat.*0.01400529697297297297297297297297);

figure(1)
hold on
yyaxis left
plot(T, Current)
yyaxis right
plot(T, Vbat)
hold off

dutys = [20 30.5 40 50 60 61 70 80 89.5 90].';


for j = 1:size(dutys)
    x = find(Duty == dutys(j))
    figure(2)
    hold on
    plot(RPM(x), W(x), '*')
    hold off   
end
figure(2)

xlim([0 400]);
legend(num2str(dutys),'Location','northwest')
%%%legend({'cos(x)','cos(2x)'},'Location','southwest')
title(legend,'Impulso skverbtis, %')
xlabel('Apsisukimai per min., RPM');
ylabel('Srovë, A')






