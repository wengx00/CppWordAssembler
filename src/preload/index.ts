import { electronAPI } from '@electron-toolkit/preload'
import { contextBridge } from 'electron'
import ffi from 'ffi-napi'
import { readFile, writeFileSync } from 'fs'
import path from 'path'
import { app } from '@electron/remote'


const nativePath = 
  process.env.NODE_ENV === 'development' ? 
  path.resolve(__dirname, "../../core/build/libassembler.dylib") :
  path.resolve(__dirname, "../../../native/libassembler.dylib")
const assembleLib = ffi.Library(nativePath, {
  'assemble': ['bool', ['string', 'string']],
})
const root = app.getPath('temp')

// Custom APIs for renderer
const api = {
  assemble: (input: string) => new Promise((resolve, reject) => {
    input = input.replace(/\\/g, '\\\\')
    const output = path.resolve(root, "./tmp.json")
    const status = assembleLib.assemble(input, output)
    if (!status) {
      reject("结果写入错误")
    }
    readFile(output, (err, data) => {
      if (err) return reject(err)
      const res = JSON.parse(data.toString('utf-8'))
      resolve(res)
    })
  }),
  assembleByInput: (text: string) => new Promise((resolve, reject) => {
    const input = path.resolve(root, "./tmp-input")
    writeFileSync(input, text, { encoding: 'utf-8' })
    const output = path.resolve(root, "./tmp.json")
    const status = assembleLib.assemble(input, output)
    if (!status) {
      reject("结果写入错误")
    }
    readFile(output, (err, data) => {
      if (err) return reject(err)
      const res = JSON.parse(data.toString('utf-8'))
      resolve(res)
    })
  }),
}

// Use `contextBridge` APIs to expose Electron APIs to
// renderer only if context isolation is enabled, otherwise
// just add to the DOM global.
if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', api)
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (define in dts)
  window.electron = electronAPI
  // @ts-ignore (define in dts)
  window.api = api
}
