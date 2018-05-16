close all;
clear all;
clc;
% load('Nm');
load('Nm2');
D.Current = (-44.4+D.Current*0.0217 +0.6);
D.Vbat = D.Vbat.*0.01400529697297297297297297297297;
W = D.Current.* D.Vbat;
omega = (2*pi*D.Speed)/60; %D.Speed*6.28*pi*0.48

x = find (D.Speed > 200);
D.Speed(x) = NaN;

yyaxis left
plot(D.T, D.Current.* D.Vbat );
yyaxis right
plot(D.T, D.Speed );




% 
% xmazas = find((D.T > {'16-May-2018 14:00:57'} & D.T < {'16-May-2018 14:01:04'}));
% plot(D.T(xmazas), D.Current(xmazas).* D.Vbat(xmazas) );
% 
% xvid = find((D.T > {'16-May-2018 14:04:48'} & D.T < {'16-May-2018 14:04:58'}));
% plot(D.T(xvid), D.Current(xvid).* D.Vbat(xvid) );

Nmm = W ./ D.Speed;%%omega;
yyaxis left
plot(D.T, W );
yyaxis right
plot(D.T, Nmm );

% yyaxis left
% plot(D.T(xvid), W(xvid) );
% yyaxis right
% plot(D.T(xvid), D.Speed(xvid) );
