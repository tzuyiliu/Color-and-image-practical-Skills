function varargout = HW1(varargin)
% HW1 MATLAB code for HW1.fig
%      HW1, by itself, creates a new HW1 or raises the existing
%      singleton*.
%
%      H = HW1 returns the handle to a new HW1 or the handle to
%      the existing singleton*.
%
%      HW1('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in HW1.M with the given input arguments.
%
%      HW1('Property','Value',...) creates a new HW1 or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before HW1_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to HW1_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help HW1

% Last Modified by GUIDE v2.5 31-Mar-2020 11:49:17

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @HW1_OpeningFcn, ...
                   'gui_OutputFcn',  @HW1_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before HW1 is made visible.
function HW1_OpeningFcn(hObject, eventdata, handles, varargin)
%只保留Open的按鈕，其他物件的visible設為off
set([handles.pushbutton2 handles.text2 handles.text3 handles.popupmenu1 handles.edit1 handles.axes1], 'visible', 'off')

% Choose default command line output for HW1
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes HW1 wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = HW1_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
%設定global變數 roi
global roi; 
%顯示選檔介面且圖片格式為jpg或bmp，介面名稱為Select an image
[filename, pathname, filterindex]  = uigetfile({'*.jpg';'*.bmp'},'Select an image'); 
%讀取影像並存入im變數中
im = imread([pathname filename]); 
%讀取im圖像的長寬以及通道數
[width height zz] = size(im);
%設定除了Open按鈕外的物件的visible為on
set([handles.pushbutton2 handles.text2 handles.text3 handles.popupmenu1 handles.edit1 handles.axes1], 'visible', 'on')
%秀圖
imshow(im) 
%透過ginput函數存取兩次click的座標
[y x] = ginput(2);

%將click的x座標小於1的設為1，大於寬的設成寬
x( find(x<1) ) = 1;
x( find(x>width) ) = width;
%將click的y座標小於1的設為1，大於長的設成長
y( find(y<1) ) = 1;
y( find(y>height) ) = height;

%將x y的數值四捨五入成整數
x = round(x);
y = round(y);
%將roi設定成所截出的圖片
roi = im(min(x):max(x), min(y):max(y), :);
%秀圖
imshow(roi);


% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
%設定global變數 roi
global roi;
%從edit1的string中讀取秒數
sec = str2num(get(handles.edit1, 'string'));
%從popupmenu1讀取要做淡入黑還是淡入白
op = get(handles.popupmenu1, 'value');

%準備一個寫入影片的物件
vidObj = VideoWriter('output.mp4', 'MPEG-4');
%打開影片物漸
open(vidObj);

%判斷若秒數大於0才去做成影片
 if(sec > 0)
    for k = 1:30*sec
        switch op
            %case 1 是淡入白
            case 1
                currFrame = (1 - k/(sec*30))*roi + k/(sec*30)*255;
            %case 2 是淡入黑
            case 2
                currFrame = (1 - k/(sec*30))*roi + k/(sec*30)*0;
        end
        %把每個frame寫入影片
       writeVideo(vidObj,currFrame);
    end
 %否則設定edit1的string為請輸入大於0的正整數，且不做寫影片的動作
 else
     set(handles.edit1, 'string', '請輸入大於0的正整數');
 end
 %關閉影片物件
 close(vidObj);
 %關閉視窗
 close();


% --- Executes on selection change in listbox1.
function listbox1_Callback(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns listbox1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from listbox1


% --- Executes during object creation, after setting all properties.
function listbox1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to listbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double


% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
