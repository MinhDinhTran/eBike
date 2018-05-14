close all;
clear all;
clc;
load('PlotPedalData');
% load('D');



figure(1)
hold on
yyaxis left
plot(Data.PedalL.T, Data.PedalL.Raw)
yyaxis right
plot(Data.PedalR.T, Data.PedalR.Raw )
hold off

% dutys = [50 60 70 90].';
% 
% for j = 1:size(dutys)
%     x = [];
%     y = [];
%     y2 = [];
%     index = 1;
%     s = size(D.T);
%     for i = 1:s(2)-1
%         if (D.Duty(i) == dutys(j))
%             x(index) = D.Speed(i);
%             y(index) = (Maxx.PedalL(i) - Minn.PedalL(i)) / (D.Current(i)*D.Vbat(i));
%             y2(index) = (Maxx.PedalR(i) - Minn.PedalR(i)) / (D.Current(i)*D.Vbat(i));
%             index = index + 1;
%         end
%     end
%     subplot(4,1,j)
%     hold on
%     yyaxis left
%     plot(x, y,'*')
%     xlim([0 200])
%     ylim([0 0.0002])
%     yyaxis right
%     plot(x, y2,'*')
%     xlim([0 200])
%     ylim([0 0.0002])
%     legend(num2str(dutys(j)))
%     hold off  
%     
% end
% 
% 


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
dif = 1./(dif*datenum(seconds(60)));
RPM = zeros(size(Data.PedalL.T));
RPM(locs(1:end-1)) = dif;
RPM(RPM==0) = NaN;
RPM = fillmissing(RPM,'previous');




pp = zeros(size(Data.PedalL.T));
pp(locs) = p;
% pp(pp==0) = NaN;
% pp = fillmissing(pp,'previous');


% Data.PedalL.D = Data.PedalL.Raw.*Data.PedalL.Raw.*(-0.021865445939308)+(6.884278410655374.*Data.PedalL.Raw)
% Data.PedalR.D = Data.PedalR.Raw.*Data.PedalR.Raw.*(-0.006791354486600)+(3.551341158326144.*Data.PedalR.Raw) 
pp = pp/2.804292164800153;
figure(1)
ax1= subplot(4,1,1)
plot(Data.PedalL.T,Data.PedalL.Raw,'-v','MarkerIndices',locs)
xlabel('Laikas')
ylabel('ADC duomenys')

ax2 = subplot(4,1,2)
plot(Data.PedalL.T, pp);
xlabel('Laikas')
ylabel('Mynimo j?ga, N')

ax3 = subplot(4,1,3)
plot(Data.PedalL.T, RPM);
xlabel('Laikas')
ylabel('Myni? skai?ius per minut?')

ax4 = subplot(4,1,4)
plot(Data.PedalL.T, pp.*RPM.*(0.17*2*pi/60));
xlabel('Laikas')
ylabel('Mynimo galia, W')

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
ax1 = subplot(4,1,1)
plot(D.T, Vbat);
ax2 = subplot(4,1,2)
plot(D.T, I);
ax3 = subplot(4,1,3)
plot(D.T, W);
ax4 = subplot(4,1,4)
plot(D.T, Wh);

linkaxes([ax1,ax2,ax3,ax4],'x')


