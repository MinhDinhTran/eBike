dutys = [50 60 70 80 90].';

% figure(12)
for j = 1:size(dutys)
    x = [];
    y = [];
    y2 = [];
    index = 1;
    s = size(D.T);
    for i = 1:s(2)-1
        if (D.Duty(i) > dutys(j)-1 && D.Duty(i) < dutys(j)+1 )
            x(index) = D.Speed(i);
            y(index) = Wh_m(index) ;
%             y(y<-100) = 0;
%             y(isnan(y)) = 0;
            index = index + 1;
        end
    end
    figure(12)
%     yyaxis left
    hold on
    plot(x, y,'*')
    hold off  
%     yyaxis right
%     hold on
%     plot(x, y2,'*')
%     hold off  
end
    legend(num2str(dutys))
    
    