close all;
clear all;
clc;
% ParsePedalData();
load('PlotPedalData');




% % % % % % figure(3) 
% % % % % % hold on
% % % % % % % 
% % % % % % % yyaxis left
% % % % % % % plot(Data.PedalL.T, y1,'b');
% % % % % % yyaxis right
% % % % % % plot(Data.PedalL.T, Data.PedalL.Raw,'r');
% % % % % % 
% % % % % % hold off



figure(1)
hold on
yyaxis left
plot(Data.PedalL.T, Data.PedalL.Raw)
yyaxis right
plot(Data.PedalR.T, Data.PedalR.Raw )
hold off



figure(2)
ax1= subplot(5,1,1);
plot(Data.PedalL.T,Data.PedalL.Raw,'-v','MarkerIndices',locs)
xlabel('Laikas')
ylabel('ADC duomenys')

ax2 = subplot(5,1,2);
plot(Data.PedalL.T, N);
xlabel('Laikas')
ylabel('Mynimo j?ga, N')

ax3 = subplot(5,1,3);
plot(Data.PedalL.T, RPM);
xlabel('Laikas')
ylabel('Myni? skai?ius per minut?')

ax4 = subplot(5,1,4);
plot(D.T, W_m);
xlabel('Laikas')
ylabel('Mynimo galia, W')

% ax4 = subplot(5,1,5);
% plot(Data.PedalL.T, Wh_m);
% xlabel('Laikas')
% ylabel('Mynimo, Wh')

linkaxes([ax1,ax2,ax3,ax4],'x')
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
plot(D.T, N_10s);
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

