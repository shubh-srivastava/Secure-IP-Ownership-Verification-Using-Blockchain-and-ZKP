import { useEffect, useState } from "react";

export default function Blockchain() {
  const [chain, setChain] = useState([]);

  useEffect(() => {
    fetch("http://localhost:18080/chain")
      .then(r => r.json())
      .then(setChain);
  }, []);

  return (
    <div>
      <h2>Blockchain</h2>
      {chain.map((b, i) => (
        <pre key={i}>{JSON.stringify(b, null, 2)}</pre>
      ))}
    </div>
  );
}
