.PHONY: docs clean-docs

docs:
	doxygen Doxyfile > doxy.log 2>&1

clean-docs:
	rm -rf html/ latex/ doxy.log