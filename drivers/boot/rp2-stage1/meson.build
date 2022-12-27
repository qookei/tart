c = find_program('c')

rp2_stage1_blob_gen = generator(
	find_program('make-stage1-blob.py'),
	arguments : [c.full_path(), objcopy.full_path(), '@INPUT@', '@OUTPUT@'],
	output : 'blob-rp2-@BASENAME@.h')

rp2_stage1_w25q080_lib = static_library('tart-rp2-stage1-w25q080',
	'stage1.cpp', rp2_stage1_blob_gen.process('stage1-w25q080.S'),
	dependencies : tart_core_dep,
	pic : false)

rp2_stage1_dep = declare_dependency(link_whole : rp2_stage1_w25q080_lib)
