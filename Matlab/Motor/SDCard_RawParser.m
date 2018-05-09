close all;
clear all;
clc;

% path = './Test 1/';
% 

path = './20180507_2/';
path = './20180508_2/';
path = './New Folder/';
path = './New Folder (2)/';

listing = dir(path);
for fileNo = 3:size(listing)
    file = strcat(path,listing(fileNo).name);
    fileID = fopen(file,'r');
    data = fread(fileID,19,'*char');
    dataN = datetime(data.','InputFormat','yyyy-MM-dd HH:mm:ss')
    
    
    
    A = fread(fileID,'uint16','l');
    sizeV = size(A);
    len = fix(sizeV(1)/2);
    
    dateArray =  linspace(dataN,dataN+seconds((len-1)*1/100),len-1).';
    
    % plot (A);
    figure(1)
%      clf(1)
%     ylim([1700 2200])
     hold on
%      plot (dateArray, A(1:len-1) * 0.01356534,'blue');
%      plot (dateArray, -44.4+(A(len+1:len*2-1)*0.0217),'red');
     plot (dateArray, A(1:len-1),'blue');
     plot (dateArray, (A(len+1:len*2-1)),'red');
%      plot (dateArray + seconds(1600/24000), (A(len+1:len*2-1)),'black');

%      plot (dateArray,  min(A(1:len-1) , (A(len+1:len*2-1))),'red');
    
    hold off
    pause(0.5);
    fclose('all')
end
