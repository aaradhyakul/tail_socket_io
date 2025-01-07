import { useEffect, useRef, useState } from 'react'
import './App.css'
import {socket} from './socket'


function connect(){
	socket.connect();
}

function App() {
	const [isConnected, setIsConnected] = useState<boolean>(socket.connected);
	const logsDivRef = useRef<HTMLDivElement | null>(null);
	useEffect(() => {
		function onConnect(){
			setIsConnected(true);
		}
		function onDisconnect(){
			setIsConnected(false);
		}
		function onLogUpdate(data: string){
			if(logsDivRef && logsDivRef.current){
				logsDivRef.current.textContent = data;
			}
		}

		socket.on('connect', onConnect);
		socket.on('disconnect', onDisconnect);
		socket.on('log_update', (data) => onLogUpdate(data));

		return () => {
			socket.off('connect', onConnect);
			socket.off('disconnect', onDisconnect);
			socket.on('logs_update', onLogUpdate);
		}
	})

  return (
	<>
	<div>
	TAIL 
	</div>
	<button onClick={connect}>Listen for logs</button>
	<div className='logs' ref={logsDivRef}></div>
	</>
  )
}

export default App
