close all;
clear all;
clc;
load('Data');



x = find(Data.Duty.T(2:end,:)-Data.Duty.T(1:end-1,:)<seconds(1));
Data.Duty.Raw(x) = NaN;

% x = find(Data.Duty.Raw==50);
for i = 1:size(Data.Duty.T)-1
    Data.Duty.D(Data.PedalL.T>Data.Duty.T(i) & Data.PedalL.T<Data.Duty.T(i+1)) = Data.Duty.Raw(i);
end

% 
% figure(1)
% hold on
% yyaxis left
% plot(Data.PedalL.T, Data.PedalL.Raw)
% yyaxis right
% plot(Data.PedalL.T, Data.Duty.D)
% hold off



legendie = strings(1,10);
for i = 1:10
    figure(1)
    hold on
    x = find(Data.Duty.D == i*10);
    a = find(Data.PedalL.T(x(:,2:end))-Data.PedalL.T(x(:,1:end-1)) > seconds(3))
    plot(Data.PedalL.T(x), Data.PedalL.Raw(x)) 
%     plot(Data.PedalL.T(x(:,1:a)), Data.PedalL.Raw(x(:,1:a)))  
%     plot(Data.PedalL.T(x(:,a+1:end)), Data.PedalL.Raw(x(:,a+1:end))) 

%     if(isempty(a))
        G.(strcat('D_',num2str(i))) = Data.PedalL.Raw(x);
%     else
%         G.(strcat('D_',num2str(i))) = Data.PedalL.Raw(x(:,1:a));
%         G.(strcat('D_',num2str(i+10))) = Data.PedalL.Raw(x(:,a+1:end));
%     end
%     
    
    hold off
    legendie(i)=num2str(i*10);
end

xx = [10 20 30 40 50 60 70 80 90 100];%% 10 20 30 40 50 60 70 80 90];
yy = [];
for i = 1:10
    hold on
    yy(i) = mean(G.(strcat('D_',num2str(i))));
    
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

