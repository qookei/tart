
objcopy = find_program('objcopy')

elf = executable('hello.elf', 'main.cpp',
		dependencies: tart_dep)

bin = custom_target('hello.bin',
		input: elf, output: 'hello.bin',
		build_by_default: true,
		command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'])
