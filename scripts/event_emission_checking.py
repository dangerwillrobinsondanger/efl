#!/usr/bin/python3

from __future__ import print_function
import argparse
import sys
import json
import subprocess
import clang.cindex
from  clang.cindex import CursorKind
import concurrent.futures
import mmap
import os

sys.path.insert(0, '../src/scripts/pyolian')
import eolian
from eolian import Eolian_C_Type_Type
from eolian import Eolian_Type_Type
from eolian import Eolian_Type_Builtin_Type

types = {}

class Source_Tree:
  def __init__(self, builddir):
    meson_introspect = subprocess.Popen(["meson", "introspect", builddir, "--targets"],
        stdout = subprocess.PIPE,
        stderr = subprocess.PIPE,
    )
    meson_introspect.poll()
    self.build_targets = json.loads(meson_introspect.stdout.read())

  def __iter__(self):
    self.source_config_tuples = []
    for build_target in self.build_targets:
      bt = build_target["type"]
      if "library" in bt:
        for lang in build_target["target_sources"]:
          if lang["language"] != "c":
            continue
          parameters = lang["parameters"]
          for source_file in lang["sources"]:
            t = {
              "parameters" : parameters,
              "sources" : source_file
            }
            self.source_config_tuples += [t]
    self.maxx = len(self.source_config_tuples)
    return self

  def __next__(self):
    if len(self.source_config_tuples) > 0:
      return self.source_config_tuples.pop()

def cursor_print(c):
  print(str(c.kind) + " " + c.displayname)
  for child in c.get_children():
    cursor_print(child)

def fetch_event(cursor):
  if cursor.kind == CursorKind.DECL_REF_EXPR:
    return cursor
  for child in cursor.get_children():
    l = fetch_event(child)
    if l is not None:
      return l
  return None

def detect_null(cursor, sourcemap):
  if cursor.kind != CursorKind.PAREN_EXPR:
    return False
  paren_expr_children = [l for l in cursor.get_children()]
  if len(paren_expr_children) != 1:
    return False
  cstyle_cast_expr = paren_expr_children[0]
  cstyle_cast_expr_children = [l for l in cstyle_cast_expr.get_children() if l.kind != CursorKind.TYPE_REF]
  if len(cstyle_cast_expr_children) != 1:
    return False
  if cstyle_cast_expr_children[0].kind != CursorKind.INTEGER_LITERAL:
    return False
  r = cstyle_cast_expr_children[0].extent
  if sourcemap.find(b"NULL", r.start.offset - 1, r.end.offset + 1):
    return True

def fetch_type(cursor):
  if cursor.kind == CursorKind.PAREN_EXPR:
    children = [l for l in cursor.get_children()]
    #cursor should be only 1 thing
    if len(children) != 1:
      print("Error, expected only 1 child "+str(len(children)))
      return
    return fetch_type(children[0])
  elif cursor.kind == CursorKind.CSTYLE_CAST_EXPR:
    children = [l for l in cursor.get_children() if l.kind != CursorKind.TYPE_REF]
    #cursor should be only 1 thing
    if len(children) != 1:
      print("Error, expected only 1 child "+str(len(children)))
      return
    return fetch_type(children[0])
  else:
    return cursor

def verify_event(cursor, event_symbol_name, cursor_of_node, ev_cursor,s):
  evtype = str(cursor_of_node.type.spelling)
  if not event_symbol_name.displayname in types:
    print("{}: ERROR event {} not defined by eolian".format(cursor.location.file, event_symbol_name.displayname))
    return
  real_ev = types[event_symbol_name.displayname]

  if evtype in real_ev or (detect_null(ev_cursor, s) and "NULL-ALLOWED" in real_ev):
    return

  print("{}:{}: {} vs. {}".format(cursor.location.file, cursor.location.line, evtype, real_ev))


def handle_check_event_callback_call(cursor, s):

  if cursor.displayname != "efl_event_callback_call":
    return
  #children should be 3
  children = [l for l in cursor.get_children()]
  if len(children) != 4:
    print("Error, different to 3 attributes "+str(len(children)))
    return
  #we ignore the first one, its just the object
  #the second one is a event
  #the third one is the event type we pass in
  event = fetch_event(children[2])
  event_type = fetch_type(children[3])
  if event is None or event_type is None:
    return "Error, {}".format(str(cursor.location))
  else:
    verify_event(cursor, event, event_type, children[3], s)
    return "{} {} {}".format(str(cursor.location), event.displayname, event_type.displayname)
  return None


def cursor_handle(c, s):
  result = ""
  if c.kind == clang.cindex.CursorKind.CALL_EXPR:
    l = handle_check_event_callback_call(c, s)
    if l is not None:
      result += l
  for child in c.get_children():
    result += cursor_handle(child, s)
  return result

def handle_file(x):
  if os.stat(x["sources"]).st_size==0:
    return ""
  with open(x["sources"], 'rb', 0) as file:
    with mmap.mmap(file.fileno(), 0, access=mmap.ACCESS_READ) as s:
      if s.find(b'efl_event_callback_call') == -1:
        return x["sources"]+" empty"

      tu = clang.cindex.TranslationUnit.from_source(
                        filename=x["sources"],
                        args=["-std=c11", "-x", "c", "-I/usr/lib/clang/7.0.1/include"] + x["parameters"],
                        unsaved_files=None,
                        options=0)
    #  for l in tu.diagnostics:
    #    print(l)
      l = cursor_handle(tu.cursor, s)
      return x["sources"]+" contains"

FLIGHTSIZE = 20
c = 0
script_path = os.path.dirname(os.path.realpath(__file__))
root_path = os.path.abspath(os.path.join(script_path, '..'))
SCAN_FOLDER = os.path.join(root_path, 'src', 'lib')

if __name__ == "__main__":
  eolian_db = eolian.Eolian_State()
  print(SCAN_FOLDER)
  if not eolian_db.directory_add(SCAN_FOLDER):
    raise(RuntimeError('Eolian, failed to scan source directory'))

  # Parse all known eo files
  if not eolian_db.all_eot_files_parse():
    raise(RuntimeError('Eolian, failed to parse all EOT files'))

  if not eolian_db.all_eo_files_parse():
    raise(RuntimeError('Eolian, failed to parse all EO files'))

  for kl in eolian_db.classes:
    for ev in kl.events:
      if ev.type is None:
        typee = ["int"]
      else:
        if ev.type.class_ != None:
          typee = ["void *", str(ev.type.c_type_get(Eolian_C_Type_Type.DEFAULT)), "NULL-ALLOWED"]
        else:
          native_type = str(ev.type.c_type_get(Eolian_C_Type_Type.DEFAULT))
          if ev.type.builtin_type > Eolian_Type_Builtin_Type.INVALID and ev.type.builtin_type < Eolian_Type_Builtin_Type.VOID:
            native_type += " *"
          typee = ["void *", native_type]
          if ev.type.builtin_type >= Eolian_Type_Builtin_Type.MSTRING and ev.type.builtin_type <= Eolian_Type_Builtin_Type.STRINGSHARE:
            typee += ["NULL-ALLOWED"]
      types["_"+ev.c_name] = typee

  with concurrent.futures.ThreadPoolExecutor(max_workers=10) as executor:
    print("Loading Meson")
    s = Source_Tree('../build/')
    si = iter(s)
    print("Submitting jobs")
    futures = {}
    run = True
    while run:
      for i in range(1,FLIGHTSIZE - len(futures)):
        file = next(si, None)
        if file is None:
          run = False
          break
        job = executor.submit(handle_file, file)
        futures[job] = job

      for future in concurrent.futures.as_completed(futures):
          res = future.result()
          if res != "":
            print("{}/{}: {}".format(c, s.maxx, res))
          c = c + 1
          del futures[future]
          if len(futures) <= (FLIGHTSIZE*(3/4)):
            break
