close all;
clear all;
clc;
feature('DefaultCharacterSet', 'windows-1257');
load('G');
% load('G_L');
% 

xx = [10 20 30 40 50 60 70 80 90 100];%% 10 20 30 40 50 60 70 80 90];
xx = xx*9.8;
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

p1r = polyfit(yy,xx,2);
p1 = polyfit(xx,yy,2);
y1 = polyval(p1,xx);
figure(1)
% yyaxis left
hold on
plot(xx, y1,'--red')
hold off



% load('G');
load('G_L');
% 

xx = [10 20 30 40 50 60 70 80 90 100];%% 10 20 30 40 50 60 70 80 90];
xx = xx*9.8;
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

p2r = polyfit(yy,xx,2);
p2 = polyfit(xx,yy,2);
y1 = polyval(p2,xx);
figure(1)

% yyaxis right
hold on
plot(xx, y1,'--green')
hold off

figure(1)
legend('Dešinysis pedalas', 'Aproksimuota DP', 'Kairysis pedalas', 'Aproksimuota KP')
xlabel('Spaudimo j?ga, N')
ylabel('ADC reikšmi? pokytis, bits')

figure(3)
x2 = 1:1:400;
y2 = x2.*x2.*p2r(1)+x2.*p2r(2);
plot(x2,y2)