G:
cd ../../x64/Release
caffe train --solver=../../models/VOC07/solver.prototxt -weights=../../models/VOC07/VGG_ILSVRC_16_layers_fc_reduced.caffemodel --gpu=0
pause