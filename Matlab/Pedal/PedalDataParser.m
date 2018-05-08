close all;
clear all;
clc;

% path = './Test 1/';
% path = './New Folder/';
path = './20180507_2/';
listing = dir(path);
for fileNo = 1:size(listing)-2
    file = strcat(path,listing(2+fileNo).name);
    fileID = fopen(file,'r');
    line_ex = fgetl(fileID);
    C{fileNo} = textscan(fileID,'%s %s 	%s	%s	%u');
    fclose('all');
end

% for fileNo = 1:size(listing)-2
%     for i = 1:size(C{fileNo}{2})
%     C{fileNo}{2}{i}(9) = '.';
%     C{fileNo}{2}{i}(13) = ' ';
%     end
% end

for fileNo = 1:size(listing)-2
    C{fileNo}{6} = datenum(C{fileNo}{2});
end

hold on
plot (C{2}{6},C{2}{5})
plot (C{3}{6},C{3}{5})
hold off 

% Fs = 1000;            % Sampling frequency                    
% T = 1/Fs;             % Sampling period       
% L = 51000;             % Length of signal
% t = (0:L-1)*T;        % Time vector
% % plot(1000*t(1:50),X(1:50))
% Y = fft(double(C{3}{5}));
% 
% P2 = abs(Y/L);
% P1 = P2(1:L/2+1);
% P1(2:end-1) = 2*P1(2:end-1);
% 
% f = Fs*(0:(L/2))/L;
% plot(f,P1) 
% 
% 
% b = 1;
% a = [1 -0.3 -0.2 -0.1];
% y = filter(b,a,double(C{3}{5}));
% plot(y)



% figure(2)
% plot(C{2}{5}-C{3}{5})
% 
% figure(2)
% hold on
% plot (C{2}{5})
% plot (C{3}{5})
% hold off 