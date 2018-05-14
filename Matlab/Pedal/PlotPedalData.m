close all;
clear all;
clc;
load('PlotPedalData');
% load('Data');
% load('D');

figure(1)
hold on
yyaxis left
plot(Data.PedalL.T, Data.PedalL.Raw)
yyaxis right
plot(Data.PedalR.T, Data.PedalR.Raw )
hold off


% % % % % % figure(3) 
% % % % % % hold on
% % % % % % % 
% % % % % % % yyaxis left
% % % % % % % plot(Data.PedalL.T, y1,'b');
% % % % % % yyaxis right
% % % % % % plot(Data.PedalL.T, Data.PedalL.Raw,'r');
% % % % % % 
% % % % % % hold off



[pks,locs,w,p] = findpeaks(Data.PedalL.Raw,...
    'MinPeakProminence',80,...
    'MinPeakDistance',40,...
    'Annotate','extents');

dif = diff(locs);
RPM = zeros(size(Data.PedalL.T));
RPM(locs(1:end-1)) = 1./(dif*datenum(seconds(60)));
RPM(RPM==0) = NaN;
RPM = fillmissing(RPM,'previous');




N = zeros(size(Data.PedalL.T));
N(locs) = p;


coefs = [0.002977880902536,2.717681357845816,99.586440784918140];
N = N.^2.*coefs(1) + N.*coefs(2);
W_m = N.*RPM.*(0.17*2*pi/60);
Wh_m = [];





[NN, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, RPM, seconds(10));
[MM, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, N, seconds(10));


xxx = find(D.T > {'14-May-2018 21:30:00'} & D.T < {'14-May-2018 21:34:00'});
MM(xxx) = 0;
xxx = find(D.T > {'14-May-2018 21:38:00'} & D.T < {'14-May-2018 21:47:00'});
MM(xxx) = 0;
xxx = find(D.T > {'14-May-2018 21:50:00'} & D.T < {'14-May-2018 21:52:00'});
MM(xxx) = 0;
xxx = find(D.T > {'14-May-2018 21:55:00'} & D.T < {'14-May-2018 22:00:00'});
MM(xxx) = 0;


s = size(MM);
for i = 1 : s(2)
    Wh_m(i) = trapz(MM(1:i));
end
%%%%%%%%%%%%%%%%%%%%






% N(N==0) = NaN;
% N = fillmissing(N,'previous');

% Data.PedalL.D = Data.PedalL.Raw.*Data.PedalL.Raw.*(-0.021865445939308)+(6.884278410655374.*Data.PedalL.Raw)
% Data.PedalR.D = Data.PedalR.Raw.*Data.PedalR.Raw.*(-0.006791354486600)+(3.551341158326144.*Data.PedalR.Raw) 

% for i = 1 :1: s(1)
%     Wh_m(i) = trapz(W_m(1:i));
% end

figure(1)
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
plot(Data.PedalL.T, W_m);
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


I = (D.Current.*0.0217)-44.4+0.6;
Vbat = (D.Vbat.*0.01400529697297297297297297297297);
W = I.*Vbat;
Wh = [];
s = size(W)
for i = 1 : s(2)
    Wh(i) = trapz(W(1:i));
end
figure(2)
ax1 = subplot(5,1,1);
plot(D.T, Vbat);
% ax2 = subplot(5,1,2);
% plot(D.T, I);

ax2 = subplot(5,1,2);
plot(D.T, W);
ax3 = subplot(5,1,3);
plot(D.T, MM.*NN.*(0.17*2*pi/60));
ax4 = subplot(5,1,4);
plot(D.T, -Wh/3600);
ax5 = subplot(5,1,5);
plot(D.T, Wh_m/3600);
linkaxes([ax2,ax3,ax4,ax5],'x')

% plot(-Wh, Wh_m);
% hold on
% % yyaxis left
% plot(D.T, -Wh);
% % yyaxis right
% plot(D.T, Wh_m);
% hold off

