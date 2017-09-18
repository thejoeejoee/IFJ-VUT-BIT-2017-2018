#!/usr/bin/python
import gdb.printing

_ = str

class LList(object):
    def __init__(self, structure):
        self._structure = structure

    def display_hint(self):
        return 'array'

    def children(self):
        current = self._structure['head']
        i = 0
        while current:
            item = current.dereference()
            yield _(i), item['value']
            i += 1
            current = item['next']

def lookup_type(val):
    if _(val.type) == 'LList *':
        return LList(val)
    return None


gdb.pretty_printers.append(lookup_type)
