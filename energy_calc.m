data = thingSpeakRead(1489248,'Fields',[1,2],'NumMinutes',1800,'OutputFormat','TimeTable');
fecha=[datestr(data.Timestamps, "yyyy-mm-dd HH:MM:SS")];
campos=[data.Irms data.Power];
v=size(fecha);
for i=1:v
    if num2str(campos(i,1))=="NaN"
        campos(i,1)=campos(i-1,1);
        campos(i,2)=campos(i-1,2);
    end
end
x=zeros(v(1),1);
c=campos(:,2);
for i=1:v
format long
yearToHours=str2double(fecha(i,1:4))*365*24;
monthToHours=str2double(fecha(i,6:7))*30*24;
daytoHours=str2double(fecha(i,9:10))*24;
justHours=str2double(fecha(i,12:13));
minutesToHours=str2double(fecha(i,15:16))/60;
secondToHours=str2double(fecha(i,18:19))/3600;
hours=yearToHours+monthToHours+daytoHours+justHours+minutesToHours+secondToHours;
x(i,1)= hours;
end
Kwh=trapz(x,c)/1000;
dinero=Kwh*0.1047;
thingSpeakWrite(1489248,Kwh,'Fields',[3],'WriteKey','X0RXNL42CIUK14I3')