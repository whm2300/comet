# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: comet.proto

from google.protobuf.internal import enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='comet.proto',
  package='eglcomet',
  serialized_pb='\n\x0b\x63omet.proto\x12\x08\x65glcomet\"<\n\nAdminLogin\x12\n\n\x02id\x18\x01 \x02(\t\x12\x0e\n\x06passwd\x18\x02 \x02(\t\x12\x12\n\nuser_agent\x18\x03 \x01(\t\"4\n\tAdminResp\x12\x0c\n\x04\x63ode\x18\x01 \x02(\x05\x12\x0b\n\x03msg\x18\x02 \x01(\t\x12\x0c\n\x04port\x18\x03 \x01(\x05\"9\n\x08\x43\x32SLogin\x12\n\n\x02id\x18\x01 \x02(\x03\x12\r\n\x05token\x18\x02 \x02(\t\x12\x12\n\nuser_agent\x18\x03 \x01(\t\"8\n\x08S2CLogin\x12\x0c\n\x04\x63ode\x18\x01 \x02(\x05\x12\x0b\n\x03msg\x18\x02 \x01(\t\x12\x11\n\tping_time\x18\x03 \x02(\x05\"*\n\x0cOnlineStatus\x12\n\n\x02id\x18\x01 \x03(\t\x12\x0e\n\x06status\x18\x02 \x01(\x0c\"F\n\tCtrlMedia\x12\x0b\n\x03\x63md\x18\x01 \x02(\x05\x12\x0c\n\x04\x61rgs\x18\x02 \x03(\t\x12\x11\n\targ_bytes\x18\x03 \x01(\x0c\x12\x0b\n\x03seq\x18\x04 \x01(\x05\"o\n\x08SongList\x12\x14\n\x0csong_list_id\x18\x01 \x02(\t\x12\x16\n\x0esong_list_type\x18\x02 \x02(\x05\x12\x16\n\x0esong_list_name\x18\x03 \x01(\t\x12\x1d\n\x05songs\x18\x04 \x03(\x0b\x32\x0e.eglcomet.Song\"\\\n\x04Song\x12\x0f\n\x07song_id\x18\x01 \x02(\t\x12\x11\n\tsong_name\x18\x02 \x02(\t\x12\x0e\n\x06source\x18\x03 \x02(\x05\x12\x0f\n\x07singers\x18\x04 \x01(\t\x12\x0f\n\x07pic_url\x18\x05 \x01(\t\"R\n\x03Msg\x12\x0e\n\x06msg_id\x18\x01 \x02(\x03\x12\x0c\n\x04type\x18\x02 \x02(\x05\x12\x0c\n\x04\x61rgs\x18\x03 \x03(\t\x12\x11\n\targ_bytes\x18\x04 \x01(\x0c\x12\x0c\n\x04\x64\x61te\x18\x05 \x01(\x03\"\x17\n\x08MsgQuery\x12\x0b\n\x03opt\x18\x01 \x01(\t\"\'\n\x07MsgResp\x12\x0e\n\x06msg_id\x18\x01 \x02(\x03\x12\x0c\n\x04\x63ode\x18\x02 \x02(\x05\"B\n\tMsgNotify\x12\x11\n\tmsg_count\x18\x01 \x02(\x05\x12\"\n\x0bmsg_content\x18\x02 \x03(\x0b\x32\r.eglcomet.Msg\".\n\x11ReqCheckTokenByID\x12\n\n\x02id\x18\x01 \x02(\x03\x12\r\n\x05token\x18\x02 \x02(\t\"4\n\x11RspCheckTokenByID\x12\x0f\n\x07rescode\x18\x01 \x02(\x05\x12\x0e\n\x06resmsg\x18\x02 \x02(\t\"4\n\x14ReqCheckRelationByID\x12\x0e\n\x06\x66romID\x18\x01 \x02(\x03\x12\x0c\n\x04toID\x18\x02 \x02(\x03\"7\n\x14RspCheckRelationByID\x12\x0f\n\x07rescode\x18\x01 \x02(\x05\x12\x0e\n\x06resmsg\x18\x02 \x02(\t\"\xbf\x01\n\tReqPacket\x12\x0c\n\x04mask\x18\x01 \x02(\x05\x12\x0b\n\x03udi\x18\x02 \x02(\t\x12\x0e\n\x06\x61\x63tion\x18\x03 \x03(\t\x12\x0e\n\x06params\x18\x04 \x03(\x0c\x12\r\n\x05reqNo\x18\x05 \x02(\x05\x12\r\n\x05\x63hnNo\x18\x06 \x01(\x05\x12\x0e\n\x06\x63hnPos\x18\x07 \x01(\x05\x12\x10\n\x08\x63lientId\x18\x08 \x02(\x05\x12\x11\n\tclientPos\x18\t \x01(\x05\x12\x11\n\tclientVer\x18\n \x01(\t\x12\x11\n\trsaKeyVer\x18\x0b \x01(\t\"Z\n\tRspPacket\x12\x0c\n\x04mask\x18\x01 \x02(\x05\x12\x0f\n\x07rescode\x18\x02 \x02(\x05\x12\x0e\n\x06resmsg\x18\x03 \x02(\t\x12\x0e\n\x06\x61\x63tion\x18\x04 \x03(\t\x12\x0e\n\x06params\x18\x05 \x03(\x0c\"+\n\tRspRsaKey\x12\x0e\n\x06rsaVer\x18\x01 \x02(\t\x12\x0e\n\x06rsaKey\x18\x02 \x02(\t*8\n\x08MaskCode\x12\x0b\n\x07\x44\x45\x46\x41ULT\x10\x00\x12\x0f\n\x0bPARAMS_GZIP\x10\x01\x12\x0e\n\nPARAMS_RSA\x10\x02')

_MASKCODE = _descriptor.EnumDescriptor(
  name='MaskCode',
  full_name='eglcomet.MaskCode',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='DEFAULT', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='PARAMS_GZIP', index=1, number=1,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='PARAMS_RSA', index=2, number=2,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=1343,
  serialized_end=1399,
)

MaskCode = enum_type_wrapper.EnumTypeWrapper(_MASKCODE)
DEFAULT = 0
PARAMS_GZIP = 1
PARAMS_RSA = 2



_ADMINLOGIN = _descriptor.Descriptor(
  name='AdminLogin',
  full_name='eglcomet.AdminLogin',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='eglcomet.AdminLogin.id', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='passwd', full_name='eglcomet.AdminLogin.passwd', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='user_agent', full_name='eglcomet.AdminLogin.user_agent', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=25,
  serialized_end=85,
)


_ADMINRESP = _descriptor.Descriptor(
  name='AdminResp',
  full_name='eglcomet.AdminResp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='code', full_name='eglcomet.AdminResp.code', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='msg', full_name='eglcomet.AdminResp.msg', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='port', full_name='eglcomet.AdminResp.port', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=87,
  serialized_end=139,
)


_C2SLOGIN = _descriptor.Descriptor(
  name='C2SLogin',
  full_name='eglcomet.C2SLogin',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='eglcomet.C2SLogin.id', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token', full_name='eglcomet.C2SLogin.token', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='user_agent', full_name='eglcomet.C2SLogin.user_agent', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=141,
  serialized_end=198,
)


_S2CLOGIN = _descriptor.Descriptor(
  name='S2CLogin',
  full_name='eglcomet.S2CLogin',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='code', full_name='eglcomet.S2CLogin.code', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='msg', full_name='eglcomet.S2CLogin.msg', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='ping_time', full_name='eglcomet.S2CLogin.ping_time', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=200,
  serialized_end=256,
)


_ONLINESTATUS = _descriptor.Descriptor(
  name='OnlineStatus',
  full_name='eglcomet.OnlineStatus',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='eglcomet.OnlineStatus.id', index=0,
      number=1, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='status', full_name='eglcomet.OnlineStatus.status', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=258,
  serialized_end=300,
)


_CTRLMEDIA = _descriptor.Descriptor(
  name='CtrlMedia',
  full_name='eglcomet.CtrlMedia',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='cmd', full_name='eglcomet.CtrlMedia.cmd', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='args', full_name='eglcomet.CtrlMedia.args', index=1,
      number=2, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='arg_bytes', full_name='eglcomet.CtrlMedia.arg_bytes', index=2,
      number=3, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='seq', full_name='eglcomet.CtrlMedia.seq', index=3,
      number=4, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=302,
  serialized_end=372,
)


_SONGLIST = _descriptor.Descriptor(
  name='SongList',
  full_name='eglcomet.SongList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='song_list_id', full_name='eglcomet.SongList.song_list_id', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='song_list_type', full_name='eglcomet.SongList.song_list_type', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='song_list_name', full_name='eglcomet.SongList.song_list_name', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='songs', full_name='eglcomet.SongList.songs', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=374,
  serialized_end=485,
)


_SONG = _descriptor.Descriptor(
  name='Song',
  full_name='eglcomet.Song',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='song_id', full_name='eglcomet.Song.song_id', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='song_name', full_name='eglcomet.Song.song_name', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='source', full_name='eglcomet.Song.source', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='singers', full_name='eglcomet.Song.singers', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='pic_url', full_name='eglcomet.Song.pic_url', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=487,
  serialized_end=579,
)


_MSG = _descriptor.Descriptor(
  name='Msg',
  full_name='eglcomet.Msg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='msg_id', full_name='eglcomet.Msg.msg_id', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='type', full_name='eglcomet.Msg.type', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='args', full_name='eglcomet.Msg.args', index=2,
      number=3, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='arg_bytes', full_name='eglcomet.Msg.arg_bytes', index=3,
      number=4, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='date', full_name='eglcomet.Msg.date', index=4,
      number=5, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=581,
  serialized_end=663,
)


_MSGQUERY = _descriptor.Descriptor(
  name='MsgQuery',
  full_name='eglcomet.MsgQuery',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='opt', full_name='eglcomet.MsgQuery.opt', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=665,
  serialized_end=688,
)


_MSGRESP = _descriptor.Descriptor(
  name='MsgResp',
  full_name='eglcomet.MsgResp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='msg_id', full_name='eglcomet.MsgResp.msg_id', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='code', full_name='eglcomet.MsgResp.code', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=690,
  serialized_end=729,
)


_MSGNOTIFY = _descriptor.Descriptor(
  name='MsgNotify',
  full_name='eglcomet.MsgNotify',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='msg_count', full_name='eglcomet.MsgNotify.msg_count', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='msg_content', full_name='eglcomet.MsgNotify.msg_content', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=731,
  serialized_end=797,
)


_REQCHECKTOKENBYID = _descriptor.Descriptor(
  name='ReqCheckTokenByID',
  full_name='eglcomet.ReqCheckTokenByID',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='eglcomet.ReqCheckTokenByID.id', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='token', full_name='eglcomet.ReqCheckTokenByID.token', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=799,
  serialized_end=845,
)


_RSPCHECKTOKENBYID = _descriptor.Descriptor(
  name='RspCheckTokenByID',
  full_name='eglcomet.RspCheckTokenByID',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rescode', full_name='eglcomet.RspCheckTokenByID.rescode', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='resmsg', full_name='eglcomet.RspCheckTokenByID.resmsg', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=847,
  serialized_end=899,
)


_REQCHECKRELATIONBYID = _descriptor.Descriptor(
  name='ReqCheckRelationByID',
  full_name='eglcomet.ReqCheckRelationByID',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='fromID', full_name='eglcomet.ReqCheckRelationByID.fromID', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='toID', full_name='eglcomet.ReqCheckRelationByID.toID', index=1,
      number=2, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=901,
  serialized_end=953,
)


_RSPCHECKRELATIONBYID = _descriptor.Descriptor(
  name='RspCheckRelationByID',
  full_name='eglcomet.RspCheckRelationByID',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rescode', full_name='eglcomet.RspCheckRelationByID.rescode', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='resmsg', full_name='eglcomet.RspCheckRelationByID.resmsg', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=955,
  serialized_end=1010,
)


_REQPACKET = _descriptor.Descriptor(
  name='ReqPacket',
  full_name='eglcomet.ReqPacket',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='mask', full_name='eglcomet.ReqPacket.mask', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='udi', full_name='eglcomet.ReqPacket.udi', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='action', full_name='eglcomet.ReqPacket.action', index=2,
      number=3, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='params', full_name='eglcomet.ReqPacket.params', index=3,
      number=4, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='reqNo', full_name='eglcomet.ReqPacket.reqNo', index=4,
      number=5, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='chnNo', full_name='eglcomet.ReqPacket.chnNo', index=5,
      number=6, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='chnPos', full_name='eglcomet.ReqPacket.chnPos', index=6,
      number=7, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='clientId', full_name='eglcomet.ReqPacket.clientId', index=7,
      number=8, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='clientPos', full_name='eglcomet.ReqPacket.clientPos', index=8,
      number=9, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='clientVer', full_name='eglcomet.ReqPacket.clientVer', index=9,
      number=10, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='rsaKeyVer', full_name='eglcomet.ReqPacket.rsaKeyVer', index=10,
      number=11, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1013,
  serialized_end=1204,
)


_RSPPACKET = _descriptor.Descriptor(
  name='RspPacket',
  full_name='eglcomet.RspPacket',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='mask', full_name='eglcomet.RspPacket.mask', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='rescode', full_name='eglcomet.RspPacket.rescode', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='resmsg', full_name='eglcomet.RspPacket.resmsg', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='action', full_name='eglcomet.RspPacket.action', index=3,
      number=4, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='params', full_name='eglcomet.RspPacket.params', index=4,
      number=5, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1206,
  serialized_end=1296,
)


_RSPRSAKEY = _descriptor.Descriptor(
  name='RspRsaKey',
  full_name='eglcomet.RspRsaKey',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='rsaVer', full_name='eglcomet.RspRsaKey.rsaVer', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='rsaKey', full_name='eglcomet.RspRsaKey.rsaKey', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=1298,
  serialized_end=1341,
)

_SONGLIST.fields_by_name['songs'].message_type = _SONG
_MSGNOTIFY.fields_by_name['msg_content'].message_type = _MSG
DESCRIPTOR.message_types_by_name['AdminLogin'] = _ADMINLOGIN
DESCRIPTOR.message_types_by_name['AdminResp'] = _ADMINRESP
DESCRIPTOR.message_types_by_name['C2SLogin'] = _C2SLOGIN
DESCRIPTOR.message_types_by_name['S2CLogin'] = _S2CLOGIN
DESCRIPTOR.message_types_by_name['OnlineStatus'] = _ONLINESTATUS
DESCRIPTOR.message_types_by_name['CtrlMedia'] = _CTRLMEDIA
DESCRIPTOR.message_types_by_name['SongList'] = _SONGLIST
DESCRIPTOR.message_types_by_name['Song'] = _SONG
DESCRIPTOR.message_types_by_name['Msg'] = _MSG
DESCRIPTOR.message_types_by_name['MsgQuery'] = _MSGQUERY
DESCRIPTOR.message_types_by_name['MsgResp'] = _MSGRESP
DESCRIPTOR.message_types_by_name['MsgNotify'] = _MSGNOTIFY
DESCRIPTOR.message_types_by_name['ReqCheckTokenByID'] = _REQCHECKTOKENBYID
DESCRIPTOR.message_types_by_name['RspCheckTokenByID'] = _RSPCHECKTOKENBYID
DESCRIPTOR.message_types_by_name['ReqCheckRelationByID'] = _REQCHECKRELATIONBYID
DESCRIPTOR.message_types_by_name['RspCheckRelationByID'] = _RSPCHECKRELATIONBYID
DESCRIPTOR.message_types_by_name['ReqPacket'] = _REQPACKET
DESCRIPTOR.message_types_by_name['RspPacket'] = _RSPPACKET
DESCRIPTOR.message_types_by_name['RspRsaKey'] = _RSPRSAKEY

class AdminLogin(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ADMINLOGIN

  # @@protoc_insertion_point(class_scope:eglcomet.AdminLogin)

class AdminResp(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ADMINRESP

  # @@protoc_insertion_point(class_scope:eglcomet.AdminResp)

class C2SLogin(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _C2SLOGIN

  # @@protoc_insertion_point(class_scope:eglcomet.C2SLogin)

class S2CLogin(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _S2CLOGIN

  # @@protoc_insertion_point(class_scope:eglcomet.S2CLogin)

class OnlineStatus(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ONLINESTATUS

  # @@protoc_insertion_point(class_scope:eglcomet.OnlineStatus)

class CtrlMedia(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _CTRLMEDIA

  # @@protoc_insertion_point(class_scope:eglcomet.CtrlMedia)

class SongList(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SONGLIST

  # @@protoc_insertion_point(class_scope:eglcomet.SongList)

class Song(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _SONG

  # @@protoc_insertion_point(class_scope:eglcomet.Song)

class Msg(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MSG

  # @@protoc_insertion_point(class_scope:eglcomet.Msg)

class MsgQuery(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MSGQUERY

  # @@protoc_insertion_point(class_scope:eglcomet.MsgQuery)

class MsgResp(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MSGRESP

  # @@protoc_insertion_point(class_scope:eglcomet.MsgResp)

class MsgNotify(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _MSGNOTIFY

  # @@protoc_insertion_point(class_scope:eglcomet.MsgNotify)

class ReqCheckTokenByID(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REQCHECKTOKENBYID

  # @@protoc_insertion_point(class_scope:eglcomet.ReqCheckTokenByID)

class RspCheckTokenByID(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RSPCHECKTOKENBYID

  # @@protoc_insertion_point(class_scope:eglcomet.RspCheckTokenByID)

class ReqCheckRelationByID(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REQCHECKRELATIONBYID

  # @@protoc_insertion_point(class_scope:eglcomet.ReqCheckRelationByID)

class RspCheckRelationByID(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RSPCHECKRELATIONBYID

  # @@protoc_insertion_point(class_scope:eglcomet.RspCheckRelationByID)

class ReqPacket(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _REQPACKET

  # @@protoc_insertion_point(class_scope:eglcomet.ReqPacket)

class RspPacket(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RSPPACKET

  # @@protoc_insertion_point(class_scope:eglcomet.RspPacket)

class RspRsaKey(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _RSPRSAKEY

  # @@protoc_insertion_point(class_scope:eglcomet.RspRsaKey)


# @@protoc_insertion_point(module_scope)
