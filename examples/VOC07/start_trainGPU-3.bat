D:
cd ..\..\release\MixGPU
caffe train --solver=../../models/VOC07/solver.prototxt -weights=../../models/VOC07/train_best.caffemodel --gpu=0
pause