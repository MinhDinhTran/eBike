close all;
clear all;
clc;
feature('DefaultCharacterSet', 'windows-1252') 
load('G');
% load('G_L');
% 

xx = [10 20 30 40 50 60 70 80 90 100];%% 10 20 30 40 50 60 70 80 90];
yy = [];
for i = 1:10
    yy(i) = mean(G.(strcat('D_',num2str(i))));
    err(i) = std(G.(strcat('D_',num2str(i))));
end
yy = yy - yy(1);
figure(1)
% plot(xx, yy,'*')
% yyaxis left
err(6) = err(6) * 3;
err(7) = err(7) * 3;
hold on
errorbar(xx,yy,err/2,'blue')
hold off

p = polyfit(xx,yy,1);
y1 = polyval(p,xx);
figure(1)
% yyaxis left
hold on
plot(xx, y1,'--red')
hold off



% load('G');
load('G_L');
% 

xx = [10 20 30 40 50 60 70 80 90 100];%% 10 20 30 40 50 60 70 80 90];
yy = [];
for i = 1:10
    yy(i) = mean(G.(strcat('D_',num2str(i))));
    err(i) = std(G.(strcat('D_',num2str(i))));
end

yy = yy - yy(1);
figure(1)
% plot(xx, yy,'*')
% yyaxis right
hold on
errorbar(xx,yy,err/2,'black')
hold off

p = polyfit(xx,yy,1);
y1 = polyval(p,xx);
figure(1)

% yyaxis right
hold on
plot(xx, y1,'--green')
hold off

figure(1)
legend('Dešinysis pedalas', 'Aproksimuota DP', 'Kairysis pedalas', 'Aproksimuota KP')
xlabel('Svoris, kg')
ylabel('ADC reikšmi? pokytis, bits')

