close all;
clear all;
clc;

% path = './Test 1/';
path = './New Folder/';
listing = dir(path);
for fileNo = 1:size(listing)-2
    file = strcat(path,listing(2+fileNo).name);
    fileID = fopen(file,'r');
    line_ex = fgetl(fileID);
    C{fileNo} = textscan(fileID,'%s %s 	%s	%s	%u');
    fclose('all');
end

for fileNo = 1:size(listing)-2
    C{fileNo}{6} = datenum(C{fileNo}{2});
end


hold on
plot (C{1}{6},C{1}{5})
plot (C{2}{6},C{2}{5})
hold off 

% offset = 1;
% isFirstFirst = C{1}{6}(1) > C{2}{6}(1);
% while (1)
%     offset = offset + 1;
%     if (isFirstFirst)
%         if (C{1}{6}(1) <= C{2}{6}(offset))
%             break;
%         end;
%     else
%         if (C{1}{6}(offset) >= C{2}{6}(1))
%             break;
%         end;
%     end
% end
% figure (1)
% hold on
% if isFirstFirst
%     plot (C{2}{5}(offset:end))
%     plot (C{1}{5}(1:end))
% else
%     plot (C{1}{5}(offset:end))
%     plot (C{2}{5}(1:end))
% end
% hold off 
