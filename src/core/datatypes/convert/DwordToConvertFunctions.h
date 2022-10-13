/*******************************************************************************
 * Copyright (c) 2010 - 2015 TU Vienna/ACIN, Profactor GmbH, fortiss GmbH,
 *    2018-2019 TU Vienna/ACIN
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *    Alois Zoitl, Ingo Hegny, Monika Wenger, Carolyn Oates, Patrick Smejkal,
 *    Matthias Plasch,
 *      - initial implementation and rework communication infrastructure
 *    Martin Melik-Merkumians - fixes DT_TO_TOD
 *    Martin Melik-Merkumians - removes invalid casts, update implementation
 *     to use new cast function
 *******************************************************************************/

#ifndef SRC_CORE_DATATYPES_CONVERT_DWORDTOCONVERTFUNCTIONS_H_
#define SRC_CORE_DATATYPES_CONVERT_DWORDTOCONVERTFUNCTIONS_H_

//********************************************************************************************
//   DWORD_TO_*  functions
//********************************************************************************************
inline const CIEC_DINT func_DWORD_TO_DINT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_DINT>(paVal);
}

inline const CIEC_BYTE func_DWORD_TO_BYTE(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_BYTE>(paVal);
}

inline const CIEC_INT func_DWORD_TO_INT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_INT>(paVal);
}

inline const CIEC_LINT func_DWORD_TO_LINT(const CIEC_DWORD &paVal){
  return CIEC_ANY::cast<CIEC_LINT>(paVal);
}

inline const CIEC_LWORD func_DWORD_TO_LWORD(const CIEC_DWORD &paVal){
  return CIEC_ANY::cast<CIEC_LWORD>(paVal);
}

inline const CIEC_REAL func_DWORD_TO_REAL(const CIEC_DWORD &paVal){
  return CIEC_ANY::cast<CIEC_REAL>(paVal);
}

inline const CIEC_SINT func_DWORD_TO_SINT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_SINT>(paVal);
}

inline const CIEC_UDINT func_DWORD_TO_UDINT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_UDINT>(paVal);
}

inline const CIEC_UINT func_DWORD_TO_UINT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_UINT>(paVal);
}

inline const CIEC_ULINT func_DWORD_TO_ULINT(const CIEC_DWORD &paVal){
  return CIEC_ANY::cast<CIEC_ULINT>(paVal);
}

inline const CIEC_USINT func_DWORD_TO_USINT(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_USINT>(paVal);
}

inline const CIEC_WORD func_DWORD_TO_WORD(const CIEC_DWORD &paVal) {
  return CIEC_ANY::cast<CIEC_WORD>(paVal);
}

inline const CIEC_STRING func_DWORD_TO_STRING(const CIEC_DWORD &paVal) {
  CIEC_STRING string;
  stringConverter(string, paVal);
  return string;
}

#ifdef FORTE_USE_WSTRING_DATATYPE
inline const CIEC_WSTRING func_DWORD_TO_WSTRING(const CIEC_DWORD &paVal){
  CIEC_WSTRING string;
  stringConverter(string, paVal);
  return string;
}
#endif

#endif /* SRC_CORE_DATATYPES_CONVERT_DWORDTOCONVERTFUNCTIONS_H_ */
