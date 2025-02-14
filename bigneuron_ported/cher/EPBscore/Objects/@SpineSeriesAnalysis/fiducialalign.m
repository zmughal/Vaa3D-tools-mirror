function obj=fiducialalign(obj)
global self;
global objects;
if (obj.state.firstimage) & (obj.state.secondimage)
    firststr=getappdata(obj.state.firstimage,'object');
    firstobj=eval(firststr);
    firststruct=struct(firstobj);
    secondstr=getappdata(obj.state.secondimage,'object');
    secondstruct=struct(eval(secondstr));
    for i=1:max(size(firststruct.data.dendrites))
        firstdendrite=firststruct.data.dendrites(i);
        seconddendrite=secondstruct.data.dendrites([secondstruct.data.dendrites.index]==firstdendrite.index);
        if isfield(firststruct.data,'marks') & isfield(firstdendrite,'maxint') & isfield(firstdendrite,'meanback') ... 
                & isfield(secondstruct.data,'marks') & isfield(seconddendrite,'maxint') & isfield(seconddendrite,'meanback')
            firstmarks=firststruct.data.marks([firststruct.data.marks.den_ind]==firstdendrite.index);
            secondmarks=secondstruct.data.marks([secondstruct.data.marks.den_ind]==seconddendrite.index);
            % calculate offset and stretch factor
            for j=1:max(size(firstmarks))
                firstmarks(j).length=firstdendrite.length(firstmarks(j).dendis);
            end
            for j=1:max(size(secondmarks))
                secondmarks(j).length=seconddendrite.length(secondmarks(j).dendis);
            end
            stretch=sqrt(var([secondmarks.length])/var([firstmarks.length]));
            offset=mean([secondmarks.length])-stretch*mean([firstmarks.length]);
            
            % Set the default offset on the first image in a series                
            if ~isfield(firstdendrite,'offset') | isempty(firstdendrite.offset)
                firstobj=eval(firststr);
                firstobj=set(firstobj,['data.dendrites(' num2str(i) ').offset'],0);
                firstobj=set(firstobj,['data.dendrites(' num2str(i) ').stretch'],1);
                eval([firststr '=firstobj;']);
                firststruct=struct(firstobj);
            end 
            secondobj=eval(secondstr);
            secondobj=set(secondobj,['data.dendrites(' num2str(i) ').offset'],firststruct.data.dendrites(i).offset*stretch+offset);
            secondobj=set(secondobj,['data.dendrites(' num2str(i) ').stretch'],firststruct.data.dendrites(i).stretch*stretch);
            eval([secondstr '=secondobj;']);
            
            try
            % display 2d projection overlay
            figure;
            firstmaximg=single(max(firststruct.data.ch(1).filteredArray,[],3));
            firstmaximg=(firstmaximg-firststruct.state.display.lowpixelch1)/(firststruct.state.display.highpixelch1-firststruct.state.display.lowpixelch1);
            firstmaximg(firstmaximg>1)=1;   
            firstmaximg(firstmaximg<0)=0;
            secondmaximg=single(max(secondstruct.data.ch(1).filteredArray,[],3));
            secondmaximg=(secondmaximg-secondstruct.state.display.lowpixelch1)/(secondstruct.state.display.highpixelch1-secondstruct.state.display.lowpixelch1);
            secondmaximg(secondmaximg>1)=1;             
            secondmaximg(secondmaximg<0)=0;
            
            % normalize it to a reasonable extent
            offsetxy=round(mean([secondmarks.voxel],2)-mean([firstmarks.voxel],2));
            
            
            a=zeros([size(firstmaximg) 3]);
            a(:,:,1)=firstmaximg;
            a(-min(0,offsetxy(1))+1:end-max(0,offsetxy(1)),-min(0,offsetxy(2))+1:end-max(0,offsetxy(2)),2)=secondmaximg(max(0,offsetxy(1))+1:end+min(0,offsetxy(1)),max(0,offsetxy(2))+1:end+min(0,offsetxy(2)));
            imagesc(a);
            
            catch
                lasterr
            end
            
            figure;
            firstmaxint=double(firstdendrite.maxint)/firstdendrite.meanback;
            secondmaxint=double(seconddendrite.maxint)/seconddendrite.meanback;
            secondx=(seconddendrite.length-offset)./stretch;
            plot(firstdendrite.length,firstmaxint,'b');
            hold on;
            plot(secondx,secondmaxint,'r');
            legend(num2str(double(firstdendrite.meanback)),num2str(double(seconddendrite.meanback)));
            minmaxint=min(min(firstmaxint),min(secondmaxint));
            maxmaxint=max(max(firstmaxint),max(secondmaxint));
            axis([min(0,min(secondx)-1) max(firstdendrite.length(end),max(secondx)+1) minmaxint-(maxmaxint-minmaxint)*0.05 maxmaxint+(maxmaxint-minmaxint)*0.25]);
            firstind=find([firststruct.data.spines.den_ind]==firstdendrite.index);
            firstpeaks=[firststruct.data.spines(firstind).dendis];
            firstpeaklength=[firstdendrite.length(firstpeaks)];
            for i=1:size(firstpeaks,2)
                if (firststruct.data.spines(firstind(i)).len==0)
                    plot(firstpeaklength(i),firststruct.data.spines(firstind(i)).intensity/firstdendrite.meanback,'b.');
                    h=text(firstpeaklength(i),0.1*(maxmaxint-minmaxint)+firstmaxint(firstpeaks(i)),num2str(firststruct.data.spines(firstind(i)).label));       
                else
                    plot(firstpeaklength(i),firststruct.data.spines(firstind(i)).intensity/firstdendrite.meanback,'b+');
                    h=text(firstpeaklength(i),0.1*(maxmaxint-minmaxint)+firststruct.data.spines(firstind(i)).intensity/firstdendrite.meanback,num2str(firststruct.data.spines(firstind(i)).label));              
                end
                set(h,'Color',[0 0 1]);
            end
            
            try
                secondind=find([secondstruct.data.spines.den_ind]==seconddendrite.index);
                secondpeaks=[secondstruct.data.spines(secondind).dendis];
                secondpeaklength=[seconddendrite.length(secondpeaks)];
                secondpeaklength=(secondpeaklength-offset)./stretch;
                for i=1:size(secondpeaks,2)
                    if (secondstruct.data.spines(secondind(i)).len==0)
                        plot(secondpeaklength(i),secondstruct.data.spines(secondind(i)).intensity/seconddendrite.meanback,'r.');
                        h=text(secondpeaklength(i),0.1*(maxmaxint-minmaxint)+secondmaxint(secondpeaks(i)),num2str(secondstruct.data.spines(secondind(i)).label));       
                    else
                        plot(secondpeaklength(i),secondstruct.data.spines(secondind(i)).intensity/seconddendrite.meanback,'r+');
                        h=text(secondpeaklength(i),0.1*(maxmaxint-minmaxint)+secondstruct.data.spines(secondind(i)).intensity/seconddendrite.meanback,num2str(secondstruct.data.spines(secondind(i)).label));              
                    end
                    set(h,'Color',[1 0 0]);
                end
                xlabel(stretch);
                
                figure;
                plot(double(firstdendrite.maxint/firstdendrite.meanback),'b');
                hold on;
                stretchdendis=sqrt(var([secondmarks.dendis])/var([firstmarks.dendis]));
                offsetdendis=mean([secondmarks.dendis])-stretchdendis*mean([firstmarks.dendis]);
                plot(((1:length(seconddendrite.maxint))-offsetdendis)./stretchdendis,double(seconddendrite.maxint/seconddendrite.meanback),'r');
                title('align by pixels');
            catch
                display('some boutons in second dendrite out of range');
            end
            
            if (isfield(firststruct.data,'ROI') && isfield(secondstruct.data,'ROI'))
                %                 figure;
                %                 plot(firstdendrite.length,double(firstdendrite.maxintcuberaw/firstdendrite.meanback),'b');
                %                 hold on;
                %                 plot(secondx,double(seconddendrite.maxintcuberaw/firstdendrite.meanback),'r');
                %                 title('raw normalized by ROI');
                
                figure;
                plot(firstdendrite.length,firstdendrite.maxint/firststruct.data.ROI.meanintensity,'b');
                hold on;
                plot(secondx,seconddendrite.maxint/secondstruct.data.ROI.meanintensity,'r');
                title('normalize by ROI');
                legend(num2str(firststruct.data.ROI.meanintensity),num2str(secondstruct.data.ROI.meanintensity));
            end
        end
    end
end