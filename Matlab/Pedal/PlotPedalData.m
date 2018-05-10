close all;
clear all;
clc;
load('Data');
load('D');



% figure(1)
% hold on
% yyaxis left
% plot(Data.PedalL.T, Data.PedalL.Raw)
% yyaxis right
% plot(Data.PedalR.T, Data.PedalR.Raw )
% hold off

dutys = [50 60 70 90].';

for j = 1:size(dutys)
    x = [];
    y = [];
    y2 = [];
    index = 1;
    s = size(D.T);
    for i = 1:s(2)-1
        if (D.Duty(i) == dutys(j))
            x(index) = D.Speed(i);
            y(index) = (Maxx.PedalL(i) - Minn.PedalL(i)) / (D.Current(i)*D.Vbat(i));
            y2(index) = (Maxx.PedalR(i) - Minn.PedalR(i)) / (D.Current(i)*D.Vbat(i));
            index = index + 1;
        end
    end
    subplot(4,1,j)
    hold on
    yyaxis left
    plot(x, y,'*')
    xlim([0 200])
    ylim([0 0.0002])
    yyaxis right
    plot(x, y2,'*')
    xlim([0 200])
    ylim([0 0.0002])
    legend(num2str(dutys(j)))
    hold off  
    
end




Fs = 100;            % Sampling frequency
T = 1/Fs;             % Sampling period
L = size(Data.PedalL.Raw);             % Length of signal
L = L(1);
t = (0:L-1)*T;        % Time vector
% plot(1000*t(1:50),X(1:50))
Y = fft(Data.PedalL.Raw);

P2 = abs(Y/L);
P1 = P2(1:L/2+1);
P1(2:end-1) = 2*P1(2:end-1);

f = Fs*(0:(L/2))/L;
plot(f,P1)


% % % % % % % % % % % % % % % % % %  High Pass Filter
% % % % % % % % % % % % % % Fpass = 0.1;
% % % % % % % % % % % % % % Fstop = 0.002;
% % % % % % % % % % % % % % Apass = 1;
% % % % % % % % % % % % % % Astop = 65;
% % % % % % % % % % % % % % Fs = 100;
% % % % % % % % % % % % % % % 
% % % % % % % % % % % % % % % d = designfilt('lowpassfir', ...
% % % % % % % % % % % % % % %   'PassbandFrequency',Fpass,'StopbandFrequency',Fstop, ...
% % % % % % % % % % % % % % %   'PassbandRipple',Apass,'StopbandAttenuation',Astop, ...
% % % % % % % % % % % % % % %   'DesignMethod','equiripple','SampleRate',Fs);
% % % % % % % % % % % % % % 
% % % % % % % % % % % % % % d = designfilt('highpassfir','StopbandFrequency',Fstop, ...
% % % % % % % % % % % % % %   'PassbandFrequency',Fpass,'StopbandAttenuation',Astop, ...
% % % % % % % % % % % % % %   'PassbandRipple',Apass,'SampleRate',Fs,'DesignMethod','equiripple');
% % % % % % % % % % % % % % 
% % % % % % % % % % % % % % % fvtool(d)
% % % % % % % % % % % % % % y1 =filter(d,Data.PedalL.Raw);
% % % % % % % % % % % % % % 
% % % % % % % % % % % % % % 
% % % % % % % % % % % % % % % % % % End of High Pass Filter

% y[n]=?*y[n?1]+x[n]?x[n?1]
% y1 = filter(b, a, Data.PedalL.Raw);
y1 = filter([1 -1], [1 -0.999], Data.PedalL.Raw);

figure(3) 
hold on

yyaxis left
plot(Data.PedalL.T, y1,'b');
yyaxis right
plot(Data.PedalL.T, Data.PedalL.Raw,'r');

hold off














