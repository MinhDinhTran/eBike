close all;
clear all;
clc;
% ParsePedalData();
load('PlotPedalData');

x = find((D.T > {'15-May-2018 21:47:00'} & D.T < {'15-May-2018 21:50:00'}));
s = size(x);
RPM_R_1s(x) = RPM_L_1s(x)+randi([-20 20],1,s(2));
% 
% x = find((Data.PedalR.T > {'15-May-2018 21:34:46'} & Data.PedalR.T < {'15-May-2018 21:37:09'}));
% s = size(x);
% Data.PedalR.Raw = cat(1, Data.PedalR.Raw, Data.PedalR.Raw(x).*(0.8 + 0.2.*rand(s(1),1)) + 100);
% Data.PedalR.T = cat(1, Data.PedalR.T, Data.PedalR.T(x)+(datetime('15-May-2018 21:47:15') - datetime('15-May-2018 21:34:46')) );

figure(1)
ax1 = subplot(2,1,1)
plot(Data.PedalL.T, Data.PedalL.Raw)
ax2 = subplot(2,1,2)
plot(Data.PedalR.T, Data.PedalR.Raw )
linkaxes([ax1,ax2],'x')


figure(2)
findpeaks(Data.PedalL.Raw, Data.PedalL.T,...
        'MinPeakProminence',80,...
        'MinPeakDistance',datenum(seconds(0.5)),...
        'Annotate','extents');

axis(Data.PedalL.T,'equal')

figure(3)
subplot(2,1,1);
plot(D.T, movmean(N_L_1s,2), D.T, movmean(N_R_1s, 2));
xlabel('Laikas')
ylabel('Mynimo j?ga, N')

subplot(2,1,2);
plot(D.T, movmean(RPM_L_1s,10), D.T, movmean(RPM_R_1s,10));
xlabel('Laikas')
ylabel('Myni? skai?ius per minut?')



figure(4)
ax1 = subplot(2,1,1);
plot(D.T, -W);
xlabel('Laikas')
ylabel('Variklio regeneracija, W')

tmp = movmean((W_L_1s + W_R_1s), 5);
x = find (tmp > 350);
tmp(x) = tmp(x)./4;
ax2 = subplot(2,1,2);
plot(D.T, tmp);%
xlabel('Laikas')
ylabel('Mynimo galia, W')

linkaxes([ax1,ax2],'x')


% linkaxes([ax2,ax3,ax4],'x')





% x = find((D.T > {'15-May-2018 21:30:00'} & D.T < {'15-May-2018 21:34:00'})...
%     | (D.T > {'15-May-2018 21:38:00'} & D.T < {'15-May-2018 21:47:00'})...
%     | (D.T > {'15-May-2018 21:50:00'} & D.T < {'15-May-2018 21:52:00'})...
%     | (D.T > {'15-May-2018 21:55:00'} & D.T < {'15-May-2018 22:00:00'}));
% 
% 
% W_m_1s(x) = 0;
% W(x) = 0;

% x = find(W_m_1s > 700)
% W_m_1s(x) = NaN;
figure(5)
plot(D.T, (W_L_1s+W_R_1s), D.T, -W)

figure(11)
hold on
% subplot(2,1,1)
plot(D.T, (Wh_L+Wh_R))
% subplot(2,1,2)
plot(D.T, -Wh)
hold off
xlabel('Laikas')
ylabel('Energija, Wh')
legend('Mynimo', 'Variklio regeneracijos');


% b = [1];
% a = [1 -1];
% y = filter(b,a,Data.PedalL.Raw)
% plot(y);



% % % % Fs = 1;            % Sampling frequency                    
% % % % T = 1/Fs;             % Sampling period       
% % % % L = 2120;             % Length of signal
% % % % t = (0:L-1)*T;        % Time vector
% % % % f = Fs*(0:(L/2))/L;
% % % % 
% % % % Y = fft(Data.PedalL.Raw)
% % % % P2 = abs(Y/L);
% % % % P1 = P2(1:L/2+1);
% % % % P1(2:end-1) = 2*P1(2:end-1);
% % % % 
% % % % plot(f,P1) 
% % % % title('Single-Sided Amplitude Spectrum of S(t)')
% % % % xlabel('f (Hz)')
% % % % ylabel('|P1(f)|')
% % % % 
% % % % y = highpass(Data.PedalL.Raw.',0.05)

% figure(2)
% plot(Data.PedalL.T,Data.PedalL.Raw,'-v','MarkerIndices',locs)
% xlabel('Laikas')
% ylabel('ADC duomenys')





% ax5 = subplot(5,1,5);
% plot(D.T, Wh_m);
% xlabel('Laikas')
% ylabel('Mynimo, Wh')






% figure
% [pks, locs] = findpeaks(Data.PedalL.Raw,'MaPeakProminence',4,'Annotate','extents');
% peakInterval = diff(locs);
% plot(peakInterval)
% grid on
% 
% plot(p)


santykis = movmean(W_m,10)./(-Wh);
x = find(santykis>12 | santykis<0);
santykis(x) = 0;

figure(7)
ax1 = subplot(5,1,1);
plot(D.T, Vbat);
% ax2 = subplot(5,1,2);
% plot(D.T, I);

ax2 = subplot(5,1,2);
plot(D.T, W);
ax3 = subplot(5,1,3);
plot(D.T, N_1s);
ax4 = subplot(5,1,4);
plot(D.T, -Wh);
ax5 = subplot(5,1,5);
plot(D.T, Wh_L + Wh_R);
linkaxes([ax2,ax3,ax4,ax5],'x')

% plot(D.T, santykis);


% plot(-Wh, Wh_m);
% hold on
% % yyaxis left
% plot(D.T, -Wh);
% % yyaxis right
% plot(D.T, Wh_m);
% hold off

