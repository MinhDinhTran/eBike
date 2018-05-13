close all;
clear all;
clc;
load('Data');



x = find(Data.Duty.T(2:end,:)-Data.Duty.T(1:end-1,:)<seconds(1));
Data.Duty.Raw(x) = NaN;

x = find(Data.Duty.Raw==50);
for i = 1:size(Data.Duty.T)-1
    Data.Duty.D(Data.PedalR.T>Data.Duty.T(i) & Data.PedalR.T<Data.Duty.T(i+1)) = Data.Duty.Raw(i);
end

% 
% figure(1)
% hold on
% yyaxis left
% plot(Data.PedalR.T, Data.PedalR.Raw)
% yyaxis right
% plot(Data.PedalR.T, Data.Duty.D)
% hold off



legendie = strings(1,10);
for i = 1:10
    figure(1)
    hold on
    x = find(Data.Duty.D == i*10);
    a = find(Data.PedalR.T(x(:,2:end))-Data.PedalR.T(x(:,1:end-1)) > seconds(3))
    plot(Data.PedalR.T(x), Data.PedalR.Raw(x)) 
%     plot(Data.PedalR.T(x(:,1:a)), Data.PedalR.Raw(x(:,1:a)))  
%     plot(Data.PedalR.T(x(:,a+1:end)), Data.PedalR.Raw(x(:,a+1:end))) 
    if(isempty(a))
        G.(strcat('D_',num2str(i))) = Data.PedalR.Raw(x);
    else
        G.(strcat('D_',num2str(i))) = Data.PedalR.Raw(x(:,1:a));
        G.(strcat('D_',num2str(i+10))) = Data.PedalR.Raw(x(:,a+1:end));
    end
    hold off
    legendie(i)=num2str(i*10);
end

xx = [10 20 30 40 50 60 70 80 90 100 10 20 30 40 50 60 70 80 90];
yy = [];
for i = 1:19
    hold on
    yy(i) = mean(G.(strcat('D_',num2str(i))))
    hold off
end

figure(2)
plot(xx, yy,'*')









% figure(1)
% hold on
% yyaxis left
% plot(D.T, y)
% yyaxis right
% plot(D.T, y1)
% hold off

