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
    N(locs) = p;
    Pl = zeros(size(Data.PedalL.T));
    Pl(locs) = w;

%     coefs = [0.002977880902536,2.717681357845816,99.586440784918140];
    coefs = [0.002305142887138,1.209399198107079,1.108901860932488e+02]
    N = N.^2.*coefs(1) + N.*coefs(2);
    Pl = Pl.*N;
    
    W_m = N.*RPM.*(0.17*2*pi/60);


    [RPM_1s, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, RPM, seconds(1));
    [N_1s, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, N, seconds(1));
    [Pl_1s, Minn, Maxx ]= ChangeTimeStep(Data.PedalL.T, Pl, seconds(1));
    W_m_1s = N_1s.*movmean(RPM_1s,3).*(0.17*2*pi/60);


    s = size(W_m_1s);
    Wh_m = zeros(1,s(2));
    sum = 0;
    for i = 1 : s(2)
        sum = sum + W_m_1s(i) * datenum(seconds(1));
        Wh_m(i) = sum;
    end
    I = (D.Current.*0.0217)-44.4+0.6;
    Vbat = (D.Vbat.*0.01400529697297297297297297297297);
    W = I.*Vbat;
    Wh = [];
    s = size(W);
    sum = 0;
    for i = 1 : s(2)
        sum = sum + W(i) * datenum(seconds(1));
        Wh(i) = sum;
    end
    save('PlotPedalData');
end
