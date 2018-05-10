close all;
clear all;
clc;
load('D');



% figure(1)
% hold on
% yyaxis left
% plot(D.T, D.Current)
% yyaxis right
% plot(D.T, D.Duty)
% hold off

dutys = [50 60 70 90].';

for j = 1:size(dutys)
    x = [];
    y = [];
    index = 1;
    s = size(D.T);
    for i = 1:s(2)-1
        if (D.Duty(i) == dutys(j))
            x(index) = D.Speed(i);
            y(index) = D.Current(i)*D.Vbat(i);
            index = index + 1;
        end
    end
    figure(2)
    hold on
    plot(x, y,'*')
    hold off   
end
figure(2)
legend(num2str(dutys))
