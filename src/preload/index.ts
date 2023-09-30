import { contextBridge } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'
import ffi from 'ffi-napi'
import path from 'path'
import { readFile } from 'fs'


const nativePath = 
  process.env.NODE_ENV === 'development' ? 
  path.resolve(__dirname, "../../core/build/libassembler.dylib") :
  path.resolve(__dirname, "../../../native/libassembler.dylib")
const assembleLib = ffi.Library(nativePath, {
  'assemble': ['bool', ['string', 'string']]
})

// Custom APIs for renderer
const api = {
  assemble: (input: string, output?: string) => new Promise((resolve, reject) => {
    const status = assembleLib.assemble(input, output ?? (input + ".output.json"))
    if (!status) {
      reject("结果写入错误")
    }
    readFile(output ?? (input + ".output.json"), (err, data) => {
      if (err) return reject(err)
      const res = JSON.parse(data.toString('utf-8'))
      resolve(res)
    })
  })
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
