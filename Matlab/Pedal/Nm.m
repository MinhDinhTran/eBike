close all;
clear all;
clc;
% load('Nm');
%load('Nm2');
load('nmnmn2');
D.Current = (-44.4+D.Current*0.0217 +0.6);
D.Vbat = D.Vbat.*0.01400529697297297297297297297297;
W = D.Current.* D.Vbat;
omega = (2*pi*D.Speed)/60; %D.Speed*6.28*pi*0.48

x = find (D.Speed > 200);
D.Speed(x) = NaN;

% yyaxis left
% plot(D.T, D.Current.* D.Vbat );
% yyaxis right
% plot(D.T, D.Speed );
% 



% hold on
% xmazas = find((D.T > {'17-May-2018 17:54:32'} & D.T < {'17-May-2018 17:54:42'}));
% plot(D.T(xmazas), D.Current(xmazas).* D.Vbat(xmazas) );
% 
% xvid = find((D.T > {'17-May-2018 17:56:52'} & D.T < {'17-May-2018 17:57:04'}));
% plot(D.T(xvid), D.Current(xvid).* D.Vbat(xvid) );
% 
% xmax = find((D.T > {'17-May-2018 17:58:08'} & D.T < {'17-May-2018 17:58:16'}));
% plot(D.T(xmax), D.Current(xmax).* D.Vbat(xmax) );
% hold off
figure(22)
Nmm = W ./omega;
subplot(2,1,1)
yyaxis left
plot(D.T, W );

subplot(2,1,2) 
hold on
yyaxis left
plot(D.T, Nmm );
yyaxis right
plot(D.T, D.Speed)

hold off

figure(223)

hold on
xmazas = find((D.T > {'17-May-2018 17:54:32'} & D.T < {'17-May-2018 17:54:42'}));
plot(D.Speed(xmazas), Nmm(xmazas), 'r');

xvid = find((D.T > {'17-May-2018 17:56:52'} & D.T < {'17-May-2018 17:57:04'}));
% plot(D.T(xvid), D.Current(xvid).* D.Vbat(xvid) );
plot(D.Speed(xvid), Nmm(xvid) , 'b');

xmax = find((D.T > {'17-May-2018 17:58:08'} & D.T < {'17-May-2018 17:58:16'}));
% plot(D.T(xmax), D.Current(xmax).* D.Vbat(xmax) );
plot(D.Speed(xmax), Nmm(xmax) , 'g');
hold off









% yyaxis left
% plot(D.T(xvid), W(xvid) );
% yyaxis right
% plot(D.T(xvid), D.Speed(xvid) );
