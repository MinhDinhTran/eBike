close all;
clear all;
clc;

% fileID = fopen('./First try/FILE_15.txt','r');
% fileID = fopen('./2nd try/FILE_15.txt','r');
path = './Test 1/';

for fileNo = 1:100
    file = strcat(path,'FILE_',int2str(fileNo),'.txt');
    fileID = fopen(file,'r');

    data = fread(fileID,19,'*char');
    A = fread(fileID,'uint16','l');
    sizeV = size(A);
    len = sizeV(1)/2;
    % plot (A);
    figure(1)
    clf(1)
    hold on
    plot (A(1:len-1),'blue');
    plot (A(len:len*2-1),'red');
    hold off
    pause(0.1);
end
