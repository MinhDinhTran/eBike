close all;
clear all;
clc;
% ParsePedalData();
load('PlotPedalData');


x = find((D.T > {'15-May-2018 21:30:00'} & D.T < {'15-May-2018 21:34:00'})...
    | (D.T > {'15-May-2018 21:38:00'} & D.T < {'15-May-2018 21:47:00'})...
    | (D.T > {'15-May-2018 21:50:00'} & D.T < {'15-May-2018 21:52:00'})...
    | (D.T > {'15-May-2018 21:55:00'} & D.T < {'15-May-2018 22:00:00'}));
W_m_1s(x) = 0;
W(x) = 0;

x = find(W_m_1s > 700)
W_m_1s(x) = NaN;
plot(D.T, W_m_1s, D.T, -W)

figure(11)
subplot(2,1,1)
plot(D.T, Wh_m)
subplot(2,1,2)
plot(D.T, -Wh)





% figure(1)
% hold on
% yyaxis left
% plot(Data.PedalL.T, Data.PedalL.Raw)
% yyaxis right
% plot(Data.PedalR.T, Data.PedalR.Raw )
% hold off




figure(2)
plot(Data.PedalL.T,Data.PedalL.Raw,'-v','MarkerIndices',locs)
xlabel('Laikas')
ylabel('ADC duomenys')


figure(2)
ax2 = subplot(3,1,1);
plot(D.T, N_1s);
xlabel('Laikas')
ylabel('Mynimo j?ga, N')

ax3 = subplot(3,1,2);
plot(D.T, RPM_1s);
xlabel('Laikas')
ylabel('Myni? skaišius per minut?')

ax4 = subplot(3,1,3);
plot(D.T, W_m_1s);
xlabel('Laikas')
ylabel('Mynimo galia, W')

linkaxes([ax2,ax3,ax4],'x')


ax5 = subplot(5,1,5);
plot(D.T, Wh_m);
xlabel('Laikas')
ylabel('Mynimo, Wh')






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

figure(3)
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
plot(D.T, Wh_m);
linkaxes([ax2,ax3,ax4,ax5],'x')

% plot(D.T, santykis);


% plot(-Wh, Wh_m);
% hold on
% % yyaxis left
% plot(D.T, -Wh);
% % yyaxis right
% plot(D.T, Wh_m);
% hold off

