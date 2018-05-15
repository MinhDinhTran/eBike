function [] = ParsePedalData()
    close all;
    clear all;
    clc;
    load('Data');
    load('D');


    [pks,locs,w,p] = findpeaks(Data.PedalL.Raw,...
        'MinPeakProminence',80,...
        'MinPeakDistance',40,...
        'Annotate','extents');

    RPM = zeros(size(Data.PedalL.T));
    RPM(locs(1:end-1)) = 1./(diff(locs)*datenum(seconds(60)));

    N = zeros(size(Data.PedalL.T));
    N(locs) = p;

    coefs = [0.002977880902536,2.717681357845816,99.586440784918140];
    N = N.^2.*coefs(1) + N.*coefs(2);
    
    W_m = N.*RPM.*(0.17*2*pi/60);


    [RPM_1s, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, RPM, seconds(1));
    [N_1s, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, N, seconds(1));
    W_m_1s = N_1s.*movmean(RPM_1s,3).*(0.17*2*pi/60);


% % % %     xxx = find(D.T > {'14-May-2018 21:30:00'} & D.T < {'14-May-2018 21:34:00'});
% % % %     W_m(xxx) = 0;
% % % %     xxx = find(D.T > {'14-May-2018 21:38:00'} & D.T < {'14-May-2018 21:47:00'});
% % % %     W_m(xxx) = 0;
% % % %     xxx = find(D.T > {'14-May-2018 21:50:00'} & D.T < {'14-May-2018 21:52:00'});
% % % %     W_m(xxx) = 0;
% % % %     xxx = find(D.T > {'14-May-2018 21:55:00'} & D.T < {'14-May-2018 22:00:00'});
% % % %     W_m(xxx) = 0;


%     W_m = fillmissing(W_m,'previous');
%     x = find (isnan(W_m));
%     W_m(x) = 0;
    idk(locs) = W_m(locs).*w;

    s = size(W_m);
    Wh_m = zeros(1,s(1));
    for i = 1 : s(1)
        Wh_m(i) = mean(idk) * ((Data.PedalL.T(i)-Data.PedalL.T(1))/hours(1));
    end
%     x = find (Wh_m==0);
%     Wh_m(x) = NaN;
%     Wh_m = fillmissing(Wh_m,'previous');
%     
 

    I = (D.Current.*0.0217)-44.4+0.6;
    Vbat = (D.Vbat.*0.01400529697297297297297297297297);
    W = I.*Vbat;
    Wh = [];
    s = size(W)
    for i = 1 : s(2)
        Wh(i) = mean(W(1:i)) * ((D.T(i)-D.T(1))/hours(1));
    end
    
    figure(11)
    subplot(2,1,1)
    plot(Data.PedalL.T, Wh_m)
    subplot(2,1,2)
    plot(D.T, Wh)


    save('PlotPedalData');
end
