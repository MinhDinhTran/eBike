close all;
clear all;
clc;

% path = './Test 1/';
% path = './New Folder/';20180507_1

path = './20180507_2/';
listing = dir(path);
for fileNo = 3:size(listing)
    file = strcat(path,listing(fileNo).name);
    fileID = fopen(file,'r');
    data = fread(fileID,19,'*char');
    dataN = datetime(data.','InputFormat','yyyy-MM-dd HH:mm:ss')
    
    
    
    A = fread(fileID,'uint16','l');
    sizeV = size(A);
    len = fix(sizeV(1)/2);
    
    dateArray =  linspace(dataN,dataN+seconds((len-1)*1/24000),len-1).';
    
    % plot (A);
    figure(1)
%     clf(1)
%     ylim([1700 2200])
    hold on
     plot (dateArray, A(1:len-1) * 0.01356534,'blue');
%     plot (dateArray, -44.4+(A(len+1:len*2-1)*0.0217),'red');
    hold off
    pause(0.5);
    fclose('all')
end
