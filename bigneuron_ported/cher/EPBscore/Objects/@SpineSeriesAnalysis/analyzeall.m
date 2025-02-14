function obj=analyzeall(obj)
% get the names of the images
global self;
global ogh;
global objects;
filenames=obj.state.display.filenames;
pathname=obj.state.pathname;
pathnames=obj.state.display.pathnames;
for i=1:size(filenames,2)
    [path,name,ext] = fileparts(char(pathnames(i)));
    obj.state.display.fileselection=i;
    self=struct(obj);
    ogh=self.gh;
    updateGUIbyglobal('self.state.display.fileselection');
    if (strcmp(ext,'.tif'))      
        try
            self=[];
            ogh=[];
            image=spineanalysisimageGUI;
            str=getappdata(image,'object');
            imageobj=eval(str);
            %imageobj=set(imageobj,'state.display.dualchannel',obj.state.display.dualchannel);
            imageobj=openImageByNameGUI(imageobj,[path,'\',name,ext]);
            imageobj=set(imageobj,'parameters',obj.parameters);
            imageobj=set(imageobj,'state.display.highpixel',200);
            imageobj=BatchProcess(imageobj,0);
            imageobj=savebyname(imageobj,[path,'\',name,'.spi'],'');
            %pathnames(i)=cellstr( [path '\' name '.spi']);
            destroy(imageobj);
            ogh=[];
            set(obj.gh.mainGUI.listbox,'String',pathnames);
            self=[];
        catch
            disp(['error in ' name]);
            disp(lasterr);
        end        
    end
end;
%obj=set(obj,'struct',self);             % generic object assigning part


% seriesdata= getappdata(gcbf,'seriesdata');
% filenames=get(handles.listbox,'String');
% pathname=seriesdata.pathname;
% ker=opentif('d:\research\newkernel1.tif');
% for i=1:size(filenames,1)
%     [path,name,ext] = fileparts([pathname char(filenames(i))]);
%     if (strcmp(ext,'.tif'))
%         data=initializeImageData;
%         try
%             data.imageArray=opentif([path '\' name ext]);
%         catch
%             print('Could not open image');
%             continue;
%         end
%         try
%         data.path=path;
%         data.name=name;
%     if (size(data.imageArray,3)>1)
%         tmpArray=cat(3,zeros(size(data.imageArray,1),size(data.imageArray,2)),data.imageArray,zeros(size(data.imageArray,1),size(data.imageArray,2)));
%         data.medianArray=median3d(tmpArray);
%         data.medianArray=data.medianArray(:,:,2:size(data.medianArray,3)-1);
%     else
%         data.medianArray=medfilt2(data.imageArray,[data.parameters.medianneighborhood data.parameters.medianneighborhood]);
%     end;
%     %ker=opentif('d:\research\kernel.tif');
%     if (size(data.medianArray,3)==1)
%         maxker=collapse(ker,'XY');
%         data.filteredArray=data.medianArray;
%       %  data.filteredArray=deconv3d(data.medianArray,uint16(maxker),struct('maxITER',data.parameters.deconvolutioniteration,'iresx',0.088,'iresy',0.088,'kresx',0.0365,'kresy',0.0365));
%     else
%         data.filteredArray=deconv3d(data.medianArray,uint16(ker),struct('maxITER',data.parameters.deconvolutioniteration,'iresx',0.088,'iresy',0.088,'kresx',0.0365,'kresy',0.0365));
%     end
%         data.filteredArray(1:3,:,:)=deal(0);
%         data.filteredArray(1:3,1,:)=deal(0);
%         data.filteredArray(size(data.filteredArray,1)-3:size(data.filteredArray,1),:,:)=deal(0);
%         data.filteredArray(:,size(data.filteredArray,2)-3:size(data.filteredArray,2),:)=deal(0);
%         total=size(data.imageArray,3);
%         for i = 1:total
% 	        data.binaryArray(:,:,i) = data.filteredArray(:,:,i)>mean(reshape(data.filteredArray,prod(size(data.filteredArray)),1))*data.parameters.binarizethreshold;
%         end
%         data.burnArray=burn(data.binaryArray);
%         data.maset=ma(data.burnArray);
%         data.trimset=trim(data.burnArray,data.maset,struct('correct_trim',0,'ll_path_tol',data.parameters.isolatedpathtol,'bl_path_tol',data.parameters.branchpathtol,'use_fast',1,'keep_bdry',1,'keep_zbdry',1,'do_merge',1,'ntol',data.parameters.looptol));
%         data.dendrites=dendritebackbone(data.burnArray,data.trimset,struct('maxkinkangle',data.parameters.maxkinkangle,'maxnumden',data.parameters.maxdendritenum,'maxloop',data.parameters.maxlooplength));
%         [len,order]=sort([data.dendrites.length]);
%         data.dendrites=data.dendrites(flipdim(order,2));
%         for i=1:size(data.dendrites,2)
%             data.dendrites(i).index=i;
%             data.dendrites(i).voxel(5,:)=data.dendrites(i).voxel(3,:);
%             arraysize=size(data.dendrites(i).voxel,2);
%             for j=1:arraysize %median filter of Z data to supress erratic jumps
%                 data.dendrites(i).voxel(3,j)=uint16(median(double(data.dendrites(i).voxel(5,max(1,j-10):min(j+10,arraysize)))));
%             end
%         end
%         for i=1:size(data.dendrites,2)
%         data.dendrites(i).voxel=data.dendrites(i).voxel(1:4,:);
%         end;
%         data.spines=spinedetect(data.burnArray,data.dendrites,struct('um_x',1.0,'um_y',1.0,'um_z', 1.0, 'domerge', 1, 'disc_tol', data.parameters.maxdisconnspinedis, 'minlen_tol',1.0,'brk_den',0));
%         numspines=size(data.spines,2);
%         %Calculate distances
%     for j=1:numspines
%       data.spines(j).voxels=size(data.spines(j).voxel,2);
%       %Calculate distances
%       dendrite=double(data.dendrites(data.spines(j).den_ind).voxel);
%       spine=double(data.spines(j).voxel);
%       dis=(dendrite(1,:)-spine(1,2)).*(dendrite(1,:)-spine(1,2))+(dendrite(2,:)-spine(2,2)).*(dendrite(2,:)-spine(2,2))+(dendrite(3,:)-spine(3,2)).*(dendrite(3,:)-spine(3,2));
%       [a,ind]=min(dis);
%       data.spines(j).voxel(:,1)=uint16(dendrite(1:3,ind));
%       data.spines(j).dendis=ind;
%       data.spines(j).edittype=0;    % initially no editing
%       if (size(data.spines(j).voxel,2)<10) %suppress disconnected spines that are too small
%           data.spines(j).edittype=1;
%       end;
%       if (data.spines(j).len<2.0)   %supress spines that are too close or too small
%           data.spines(j).edittype=1;
%       end;
%     end
%     spineind=[];
%     for i=1:size(data.dendrites,2)
%         loc=find([data.spines.den_ind]==i);
%         [A,B]=sort([data.spines(loc).dendis]);
%         spineind=[spineind loc(B)];
%     end
%     data.spines=data.spines(spineind);
%     data.newspinenum=1;
%     for j=1:numspines
%         data.spines(j).color=hsv2rgb([(j-5*floor(j/5))*0.2+0.2*j/numspines,1,1]);
%         data.spines(j).label=j;
%     end
%     data.slicespines={};
%     for i=1:size(data.burnArray,3)
%         ind=1;
%         for j=1:size(data.spines,2)
%             pos=find(data.spines(j).voxel(3,:)==i-1);
%             if (pos)
%                 data.slicespines(i).spines(ind)=struct('voxel',[data.spines(j).voxel(1,pos);data.spines(j).voxel(2,pos)],'index',j);
%                 ind=ind+1;    
%             end
%         end
%     end;
%     for j=1:size(data.spines,2)
%             den=data.dendrites(data.spines(j).den_ind).voxel;
%             dend=den(:,max(1,data.spines(j).dendis-5):min(size(den,2),data.spines(j).dendis+5));
%             coeff=(double([ones(1,size(dend,2)); dend(1,:)]))'\(double(dend(2,:)))';
%             data.spines(j).side=sign(double(data.spines(j).voxel(2,2))-coeff(2)*double(data.spines(j).voxel(1,2))-coeff(1));    
%     end
%     square=ones(1,16);
%     data.bigspinesr=find(([data.spines.voxels]>100)&([data.spines.side]>0));
%     data.bigspinesl=find(([data.spines.voxels]>100)&([data.spines.side]<0));
%     data.spinogramr=zeros(1,size(data.dendrites(1).voxel,2));
%     data.spinograml=data.spinogramr;
%     data.spinograml([data.spines(data.bigspinesl).dendis])=1;%[data.spines(data.bigspines).voxels]./100;
%     data.spinogramr([data.spines(data.bigspinesr).dendis])=1;
%     %  data.gaussianspinogramr=conv(data.spinogramr,gaussian);
%     %  data.gaussianspinograml=conv(data.spinograml,gaussian);
%     data.squarespinogramr=conv(data.spinogramr,square);
%     data.squarespinograml=conv(data.spinograml,square);
%     save([path '\' name '.spi'],'data');
%     catch
%         print('Failed analyzing');
%     end
%     end
